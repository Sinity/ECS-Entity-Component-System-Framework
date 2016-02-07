#include "task.h"
#include "ecs.h"

using namespace EECS;

EECS::Task::Task(ECS& engine) : engine(engine) {
    frequency = std::chrono::milliseconds(engine.config.get("task.defaultTaskFrequency", 16));
};
