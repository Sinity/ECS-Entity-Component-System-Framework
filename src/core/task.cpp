#include "task.h"
#include "engine.h"

Task::Task(Engine& engine) : engine(engine) {
    frequency = std::chrono::milliseconds(engine.config.get("task.defaultTaskFrequency", 16));
};

