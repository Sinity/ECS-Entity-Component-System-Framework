#include "task.h"
#include "ecs.h"

using namespace EECS;

TaskBase::TaskBase(ECS& ecs) : ecs(ecs) {
    frequency = std::chrono::milliseconds(ecs.config.get("task.defaultTaskFrequency", 16));
}
