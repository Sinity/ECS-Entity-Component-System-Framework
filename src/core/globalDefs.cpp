#include <unordered_map>
#include <vector>
#include "eventQueue.h"
#include "componentManager.h"
#include "taskManager.h"

size_t EventQueue::EventID::counter = 0;
size_t ComponentManager::ContainerID::counter = 0;
size_t TaskManager::TaskID::counter = 0;

std::unordered_map<std::string, size_t>& componentTypenameToContainer() {
    static std::unordered_map<std::string, size_t> typenameToContainer;
    return typenameToContainer;
}

std::vector<std::unique_ptr<ComponentContainerBase>>& singleComponentContainerArchetypes() {
    static std::vector<std::unique_ptr<ComponentContainerBase>> archetypes;
    return archetypes;
};

