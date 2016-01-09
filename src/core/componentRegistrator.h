#pragma once
#include <unordered_map>
#include "componentManager.h"
#include "globalDefs.h"

namespace EECS {
    // Used for registering component type in the system.
    // Allows for reflection stuff, like defining Entity archetypes from data.
    template<typename T>
    class ComponentRegistrator {
    public:
        ComponentRegistrator(std::string name) {
            auto id = ComponentManager::ContainerID::get<T>();
            componentTypenameToContainer()[std::move(name)] = id;

            if (singleComponentContainerArchetypes().size() <= id) {
                singleComponentContainerArchetypes().resize(id + 1);
            }

            singleComponentContainerArchetypes()[id] = std::make_unique<ComponentContainer<T>>();
        }
    };
}
