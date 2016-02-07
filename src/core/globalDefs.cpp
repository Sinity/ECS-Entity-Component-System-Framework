#include <unordered_map>
#include <vector>
#include "eventQueue.h"
#include "componentManager.h"
#include "taskScheduler.h"

using namespace EECS;

size_t EventQueue::EventID::counter = 0;
size_t ComponentManager::ContainerID::counter = 0;
size_t TaskScheduler::TaskID::counter = 0;

std::unordered_map<std::string, size_t>& EECS::componentTypenameToContainer() {
    static std::unordered_map<std::string, size_t> typenameToContainer;
    return typenameToContainer;
}

std::vector<std::unique_ptr<ComponentContainerBase>>& EECS::singleComponentContainerArchetypes() {
    static std::vector<std::unique_ptr<ComponentContainerBase>> archetypes;
    return archetypes;
};
