#pragma once
#include "componentManager.h"
#include "entityID.h"

namespace EECS {
class Entity {
   public:
    Entity(EntityID id, ComponentManager& components) : id(id), components(components) {}

    EntityID getID() { return id; }

    bool isNull() { return id == 0; }

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

    operator EntityID() { return id; }

   private:
    EntityID id;
    ComponentManager& components;
    std::pair<size_t, Component*> cachedComponent;
};
}
