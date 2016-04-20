#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <type_traits>
#include "componentContainer.h"
#include "entityID.h"
#include "globalDefs.h"
#include "componentContainerID.h"
#include "component.h"

namespace EECS {
class EntityManager;
class ComponentManager;

// Class which provides safer access to a component.
template <class ComponentType>
class ComponentHandle {
    static_assert(std::is_base_of<Component<ComponentType>, ComponentType>::value,
                  "ComponentHandle can only operate on Components");

   public:
    ComponentHandle(ComponentManager& compManager, ComponentType* component)
        : componentManager(compManager), componentPtr(component) {
        if (componentPtr) {
            entityID = component->entityID;
        }
    }

    ComponentType* operator->() const;
    operator ComponentType*() const { return operator->(); }
    ComponentType& operator*() const { return *operator->(); }

    operator bool() const { return operator->(); }

   private:
    EntityID entityID = 0;
    ComponentManager& componentManager;
    mutable ComponentType* componentPtr = nullptr;
};

// Holds all components demanded in intersection() call by pointer and provides convenient access to them by reference,
// for ex. intersectComps.get<PositionComponent>().x = 56 or bool collided = intComps.get<CollisionComp>.state;
// To get entity id which corresponds to all these components, call 'entity' method.
template <typename... ComponentTypes>
class IntersectionComponents {
   public:
    template <typename ComponentType>
    ComponentType& get() {
        return *std::get<ComponentType*>(components);
    }

    EntityID entity() { return entityID; }

   private:
    std::tuple<ComponentTypes*...> components;
    EntityID entityID;

    template <typename ComponentType>
    void set(ComponentType& component) {
        std::get<ComponentType*>(components) = &component;
    }

    friend class ComponentManager;
};

// Stores all components in the system. Provides facilities to add, delete, and get components by various methods.
class ComponentManager {
   public:
    ComponentManager() {
        containers.reserve(singleComponentContainerArchetypes().size());
        for (const auto& container : singleComponentContainerArchetypes()) {
            containers.emplace_back(container->getNewClassInstance());
        }
    }

    // Returns ComponentHandle to the created component. If it failed to create new component, handle will point to
    //  nullptr. Arguments after entityID are forwarded to constructor of the created component.
    template <class T, class... Args>
    ComponentHandle<T> addComponent(EntityID entityID, Args&&... args) {
        if (!entityExists(entityID)) {
            return ComponentHandle<T>(*this, nullptr);
        }

        auto componentPtr = getContainer<T>()->addComponent(entityID, std::forward<Args>(args)...);
        return ComponentHandle<T>(*this, componentPtr);
    }

    // Deletes component owned by given entity. Returns true if it was deleted, false if it didn't exist.
    template <class T>
    bool deleteComponent(EntityID entityID) {
        return getContainer<T>()->deleteComponent(entityID);
    }

    // Deletes all components
    void clear() {
        for (auto& container : containers) {
            container->clear();
        }
    }

    // Deletes all *T* components.
    template <class T>
    void clear() {
        getContainer<T>()->clear();
    }

    // returns pointer to component of type T, owned by entity specified by argument, or nullptr if it doesn't exists.
    template <class T>
    T* getComponent(EntityID entityID) {
        return getContainer<T>()->getComponent(entityID);
    }

    // the same as getComponent, but returns ComponentHandle instead.
    template <class T>
    ComponentHandle<T> getComponentHandle(EntityID entityID) {
        return ComponentHandle<T>(*this, getComponent<T>(entityID));
    }

    // returns reference to container which contains all components of type T. This container should not be modified in
    // any way, as this may result in breaking system's assumptions about it's state. Elements in the container
    // can be modified.
    template <class T>
    std::vector<T>& getAllComponents() {
        return getContainer<T>()->getAllComponents();
    }

    // given list of types, gets all entities which have *at least* these types and returns vector of convenient
    // helper classes that allow for access/modification of these types. Each element of vector corresponds to single
    // entity.
    // For ex.
    // [0] -> PositionComponent, MovementComponent
    // [1] -> PositionComponent, MovementComponent
    // Each element of vector have the same types(specified in intersection() call), which belong to the same entity.
    // components could be accessed like that:
    // comps.intersection<PositionComponent, MovementComponent>()[0].get<PositionComponent>().x = 5;
    // Order of Entities in returned vector is undefined.
    template <typename Head, typename... Tail>
    std::vector<IntersectionComponents<Head, Tail...>> intersection() {
        auto& headComponents = getAllComponents<Head>();

        std::vector<IntersectionComponents<Head, Tail...>> results;
        results.reserve(headComponents.size());
        std::mutex resultsMutex;

        auto worker = [&](size_t startIndex, size_t endIndex) {
            for (auto i = startIndex; i <= endIndex; i++) {
                IntersectionComponents<Head, Tail...> currentEntityRequiredComponents;
                if (fillWithRequiredComponents<IntersectionComponents<Head, Tail...>, Tail...>(
                        headComponents[i].entityID, currentEntityRequiredComponents)) {
                    currentEntityRequiredComponents.set(headComponents[i]);
                    currentEntityRequiredComponents.entityID = headComponents[i].entityID;
                    std::lock_guard<std::mutex> guard(resultsMutex);
                    results.push_back(currentEntityRequiredComponents);
                }
            }
        };

        size_t elemsPerThread = headComponents.size() / 8;

        if (elemsPerThread < 3) {
            worker(0, headComponents.size() - 1);
            return results;
        }

        std::vector<std::thread> threads;
        for (auto i = 0; i < 8; i++) {
            threads.push_back(std::thread{worker, elemsPerThread * i, elemsPerThread * (i + 1) - 1});
        }
        threads.push_back(
            std::thread{worker, headComponents.size() - headComponents.size() % 8, headComponents.size() - 1});

        for (auto& thread : threads) {
            thread.join();
        }

        return results;
    }

    // Checks if pointer to the component is still valid, in very fast way. Pointer to the component could turn invalid
    // if there was any addiction/deletion of any component which is the same type.
    template <class T>
    bool validComponentPointer(T* componentPtr, EntityID entityID) {
        auto& comps = getAllComponents<T>();
        return &comps.front() <= componentPtr && componentPtr <= &comps.back() && componentPtr->entityID == entityID;
    }

    void setEntityManager(const EntityManager& entityManager);

   private:
    std::vector<std::unique_ptr<ComponentContainerBase>> containers;
    const EntityManager* entityManager = nullptr;
    bool entityExists(EntityID entity);

    template <class T>
    ComponentContainer<T>* getContainer() {
        static_assert(std::is_base_of<Component<T>, T>::value, "T must be a component type!");
        return (ComponentContainer<T>*)containers[ComponentContainerID::get<T>()].get();
    }

    // Fills second argument with required components. Returns true if all required components belonging to given entity
    // were found
    template <typename IntersectComponents, typename Head, typename... Tail>
    bool fillWithRequiredComponents(EntityID entityID, IntersectComponents& components) {
        static_assert(std::is_base_of<Component<Head>, Head>::value,
                      "Interseciton template parameters must all be descended from Component!");

        auto currentComponent = getComponent<Head>(entityID);
        if (!currentComponent) {
            return false;
        }

        if (fillWithRequiredComponents<IntersectComponents, Tail...>(entityID, components)) {
            components.set(*currentComponent);
            return true;
        }

        return false;
    }

    template <typename IntersectComponents>
    bool fillWithRequiredComponents(EntityID, IntersectComponents) {
        return true;
    }

    template <class T>
    friend class ComponentRegistrator;
    friend class EntityManager;
    friend class Entity;
};

// implementation of method from ComponentHandle which depends on definition of ComponentManager.
template <class ComponentType>
ComponentType* ComponentHandle<ComponentType>::operator->() const {
    if (componentManager.validComponentPointer(componentPtr, entityID)) {
        return componentPtr;
    }

    componentPtr = componentManager.getComponent<ComponentType>(entityID);
    return componentPtr;
}
}
