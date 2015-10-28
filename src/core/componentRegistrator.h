#pragma once
#include <unordered_map>
#include "componentsManager.h"
#include "globalDefs.h"

// Used for registering component type in the system.
// Allows for reflection stuff, like defining Entity archetypes from data.
template<typename T>
class ComponentRegistrator {
public:
    ComponentRegistrator(std::string name) {
        auto id = ComponentsManager::ContainerID::get<T>();
        componentTypenameToContainer()[std::move(name)] = id;

        if (singleComponentContainerArchetypes().size() <= id) {
            singleComponentContainerArchetypes().resize(id + 1);
        }

        singleComponentContainerArchetypes()[id] = std::make_unique<ComponentContainer<T>>();
    }
};
