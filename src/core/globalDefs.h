#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "componentContainer.h"

namespace EECS {
std::vector<std::unique_ptr<ComponentContainerBase>>& singleComponentContainerArchetypes();
}
