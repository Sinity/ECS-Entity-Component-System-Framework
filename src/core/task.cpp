#include "task.h"
#include "ecs.h"

Task::Task(ECS& engine) : engine(engine) {
    frequency = std::chrono::milliseconds(engine.config.get("task.defaultTaskFrequency", 16));
};

