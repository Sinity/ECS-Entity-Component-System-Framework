#pragma once
#include <memory>
#include <unordered_map>
#include <utils/getTupleElementByID.h>
#include "componentContainer.h"
#include "entityID.h"
#include "globalDefs.h"

class EntityManager;

// Holds all components demanded in intersection() call by pointer and provides convenient access to them by reference,
// for ex. intersectComps.get<PositionComponent>().x = 56 or bool collided = intComps.get<CollisionComp>.state;
// To get entity which corresponds to all these components, call 'entity' method.
template<typename... ComponentTypes>
class IntersectionComponents {
public:
    template<typename ComponentType>
    ComponentType& get() {
        return *getByType<ComponentType*>(components);
    }

    EntityID entity() {
        return entityID;
    }

private:
    std::tuple<ComponentTypes* ...> components;
    EntityID entityID;

    template<typename ComponentType>
    void set(ComponentType& component) {
        getByType<ComponentType*>(components) = &component;
    }

    friend class ComponentsManager;
};

// Stores all components in the system. Provides facilities to add, delete, and get components by various methods.
class ComponentsManager {
public:
    ComponentsManager() {
        containers.reserve(singleComponentContainerArchetypes().size());
        for (const auto& container : singleComponentContainerArchetypes()) {
            containers.emplace_back(container->clone());
        }
    }

    void setEntityManager(const EntityManager& entityManager);
    bool entityExists(EntityID entity);

    template<class T>
    T* getComponent(EntityID entityID) {
        return getContainer<T>()->getComponent(entityID);
    }

    Component* getComponent(const std::string& componentTypename, EntityID entityID) {
        return getContainer(componentTypename)->genericGetComponent(entityID);
    }

    template<class T>
    std::vector<T>& getAllComponents() {
        return getContainer<T>()->getAllComponents();
    }

    template<class T, class... Args>
    T* addComponent(EntityID entityID, Args&&... args) {
        if (!entityExists(entityID)) {
            return nullptr;
        }

        return getContainer<T>()->addComponent(entityID, std::forward<Args>(args)...);
    }

    Component* addComponent(const std::string& componentTypename, EntityID entityID) {
        if (!entityExists(entityID)) {
            return nullptr;
        }

        return getContainer(componentTypename)->genericAddComponent(entityID);
    }

    template<class T>
    bool deleteComponent(EntityID entityID) {
        return getContainer<T>()->deleteComponent(entityID);
    }

    bool deleteComponent(const std::string& componentTypename, EntityID entityID) {
        return getContainer(componentTypename)->genericDeleteComponent(entityID);
    }

    void clear() {
        for (auto& container : containers) {
            container->clear();
        }
    }

    template<class T>
    void clear() {
        getContainer<T>()->clear();
    }

    void clear(const std::string& componentTypename) {
        getContainer(componentTypename)->clear();
    }

    // Checks if pointer to the component is still valid, in very fast way. Pointer to the component could turn invalid
    // if there was any addiction/deletion of any component which is the same type.
    template<class T>
    bool validComponentPointer(T* componentPointer, EntityID entityID) {
        auto& container = getAllComponents<T>();
        return &container[0] <= componentPointer && componentPointer < &container[container.size()] &&
            componentPointer->entityID == entityID;
    }

    // given list of types, gets all entities which have *at least* these types and returns vector of convenient
    // helper classes that allow for access/modification of these types. Each element of vector corresponds to single entity.
    // For ex.
    // [0] -> PositionComponent, MovementComponent
    // [1] -> PositionComponent, MovementComponent
    // Each element of vector have the same types(specified in intersection() call), which belong to the same entity.
    // components could be accessed like that:
    // comps.intersection<PositionComponent, MovementComponent>()[0].get<PositionComponent>().x = 5;
    // get() on result is very cheap O(1) method.
    template<typename Head, typename... Tail>
    std::vector<IntersectionComponents<Head, Tail...>> intersection() {
        std::vector<IntersectionComponents<Head, Tail...>> results;

        auto& headComponents = getAllComponents<Head>();
        for (auto& headComponent : headComponents) {
            IntersectionComponents<Head, Tail...> currentEntityRequiredComponents;
            if (fillWithRequiredComponents<IntersectionComponents<Head, Tail...>, Tail...>(headComponent.entityID, currentEntityRequiredComponents)) {
                currentEntityRequiredComponents.set(headComponent);
                currentEntityRequiredComponents.entityID = headComponent.entityID;
                results.push_back(std::move(currentEntityRequiredComponents));
            }
        }

        return results;
    }

private:
    std::vector<std::unique_ptr<ComponentContainerBase>> containers;
    const EntityManager* entityManager = nullptr;

    template<class T>
    ComponentContainer<T>* getContainer() {
        return (ComponentContainer<T>*)containers[ContainerID::get<T>()].get();
    }

    ComponentContainerBase* getContainer(const std::string& componentTypename) {
        return containers[componentTypenameToContainer()[componentTypename]].get();
    }

    // Fills second argument with required components. Returns true if all required components belonging to given entity were found
    template<typename IntersectComponents, typename Head, typename... Tail>
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

    template<typename IntersectComponents>
    bool fillWithRequiredComponents(EntityID, IntersectComponents) {
        return true;
    }

    class ContainerID {
    public:
        template<typename T>
        static size_t get() {
            static size_t id = counter++;
            return id;
        }

    private:
        static size_t counter;
    };

    template<class T> friend class ComponentRegistrator;
    friend class EntityManager;
    friend class Entity;
};

