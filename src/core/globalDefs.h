#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "componentContainer.h"
#include "singleEventQueue.h"

namespace EECS {
std::vector<std::unique_ptr<ComponentContainerBase>>& singleComponentContainerArchetypes();
std::vector<std::unique_ptr<SingleEventQueueBase>>& singleEventQueueArchetypes();
}
