#pragma once
#include "componentsManager.h"
#include "entityID.h"

class Entity {
public:
    Entity(EntityID id, ComponentsManager& components) : id(id), components(components) {
    }

    EntityID getID() {
        return id;
    }

    bool isNull() {
        return id == 0;
    }

    template<class T>
    T* component() {
        if (cachedComponent.first != ComponentsManager::ContainerID::get<T>() ||
                !components.validComponentPointer((T*)cachedComponent.second, id)) {
            cachedComponent = {ComponentsManager::ContainerID::get<T>(), components.getComponent<T>(id)};
        }

        return (T*)cachedComponent.second;
    }

    Component* component(const std::string& componentTypename) {
        return components.getComponent(componentTypename, id);
    }

    template<class T>
    T* addComponent() {
        return components.addComponent<T>(id);
    }

    Component* addComponent(const std::string& componentTypename) {
        return components.addComponent(componentTypename, id);
    }

    template<class T>
    bool deleteComponent() {
        return components.deleteComponent<T>(id);
    }

    bool deleteComponent(const std::string& componentTypename) {
        return components.deleteComponent(componentTypename, id);
    }

    operator EntityID() {
        return id;
    }

private:
    EntityID id;
    ComponentsManager& components;
    std::pair<size_t, Component*> cachedComponent;
};
