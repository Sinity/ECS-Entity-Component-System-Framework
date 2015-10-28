#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "componentContainer.h"

std::unordered_map<std::string, size_t>& componentTypenameToContainer();
std::vector<std::unique_ptr<ComponentContainerBase>>& singleComponentContainerArchetypes();

