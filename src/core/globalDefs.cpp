#include <unordered_map>
#include <vector>
#include "event.h"
#include "componentContainerID.h"
#include "task.h"

using namespace EECS;

size_t EventID::counter = 0;
size_t ComponentContainerID::counter = 0;
size_t TaskID::counter = 0;

std::vector<std::unique_ptr<ComponentContainerBase>>& EECS::singleComponentContainerArchetypes() {
    static std::vector<std::unique_ptr<ComponentContainerBase>> archetypes;
    return archetypes;
};

std::vector<std::unique_ptr<SingleEventQueueBase>>& EECS::singleEventQueueArchetypes() {
    static std::vector<std::unique_ptr<SingleEventQueueBase>> archetypes;
    return archetypes;
}
