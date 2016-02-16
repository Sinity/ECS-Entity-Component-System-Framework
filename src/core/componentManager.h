#pragma once
#include <memory>
#include <unordered_map>
#include "componentContainer.h"
#include "entityID.h"
#include "globalDefs.h"

namespace EECS {
class EntityManager;
class ComponentManager;

// Class which provides safer access to a component.
template <class ComponentType>
class ComponentHandle {
   public:
    ComponentHandle(ComponentManager& compManager, ComponentType* component)
        : componentManager(compManager), componentPtr(component) {
        if (componentPtr) {
            entityID = component->entityID;
        }
    }

    bool valid() const;
    operator bool() const { return valid(); }

    operator ComponentType*() const { return ptr(); }
    ComponentType* ptr() const;
    ComponentType& operator*() { return *ptr(); }
    ComponentType* operator->() const { return ptr(); };

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
    template <typename Head, typename... Tail>
    std::vector<IntersectionComponents<Head, Tail...>> intersection() {
        std::vector<IntersectionComponents<Head, Tail...>> results;

        auto& headComponents = getAllComponents<Head>();
        // TODO: parallelize it
        for (auto& headComponent : headComponents) {
            IntersectionComponents<Head, Tail...> currentEntityRequiredComponents;
            if (fillWithRequiredComponents<IntersectionComponents<Head, Tail...>, Tail...>(
                    headComponent.entityID, currentEntityRequiredComponents)) {
                currentEntityRequiredComponents.set(headComponent);
                currentEntityRequiredComponents.entityID = headComponent.entityID;
                results.push_back(std::move(currentEntityRequiredComponents));
            }
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
        return (ComponentContainer<T>*)containers[ContainerID::get<T>()].get();
    }

    // Fills second argument with required components. Returns true if all required components belonging to given entity
    // were found
    template <typename IntersectComponents, typename Head, typename... Tail>
    bool fillWithRequiredComponents(EntityID entityID, IntersectComponents& components) {
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

    class ContainerID {
       public:
        template <typename T>
        static size_t get() {
            static size_t id = counter++;
            return id;
        }

       private:
        static size_t counter;
    };

    template <class T>
    friend class ComponentRegistrator;
    friend class EntityManager;
    friend class Entity;
};

// implementation of methods from ComponentHandle which depend on definition of ComponentManager.
template <class ComponentType>
ComponentType* ComponentHandle<ComponentType>::ptr() const {
    if (componentManager.validComponentPointer(componentPtr, entityID)) {
        return componentPtr;
    }

    componentPtr = componentManager.getComponent<ComponentType>(entityID);
    return componentPtr;
}

template <class ComponentType>
bool ComponentHandle<ComponentType>::valid() const {
    return ptr() != nullptr;
}
}
