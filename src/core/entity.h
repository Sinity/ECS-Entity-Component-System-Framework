#pragma once
#include <limits>
#include "entityManager.h"
#include "entityID.h"

namespace EECS {
class Entity {
   public:
    Entity(EntityID id, EntityManager& entities, ComponentManager& components)
        : id(id), entities(entities), components(components) {}

    operator EntityID() { return id; }
    EntityID getID() { return id; }

    bool reassign(EntityID id) {
        if (!entities.entityExists(id)) {
            return false;
        }

        this->id = id;
        cachedComponent = {std::numeric_limits<size_t>::max(), nullptr};

        return true;
    }

    bool exists() { return entities.entityExists(id); }
    operator bool() { return exists(); }

    bool destroy() {
        auto result = entities.deleteEntity(id);
        id = 0;
        return result;
    }

    Entity clone() { return entities.cloneEntity(id); }

    template <class T>
    T* component() {
        if (cachedComponent.first != ComponentManager::ContainerID::get<T>() ||
            !components.validComponentPointer((T*)cachedComponent.second, id)) {
            cachedComponent = {ComponentManager::ContainerID::get<T>(), components.getComponent<T>(id)};
        }

        return (T*)cachedComponent.second;
    }

    template <class T>
    T* addComponent() {
        return components.addComponent<T>(id);
    }

    template <class T>
    bool deleteComponent() {
        return components.deleteComponent<T>(id);
    }

   private:
    EntityID id;
    EntityManager& entities;
    ComponentManager& components;
    std::pair<size_t, void*> cachedComponent;
};
}
