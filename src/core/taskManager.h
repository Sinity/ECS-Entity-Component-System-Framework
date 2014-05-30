#pragma once
#include <unordered_map>
#include "tool/profiler.h"
#include "tool/logger.h"

using TaskHandle = unsigned int;

class Engine;

class Task;

class TaskManager {
public:
    sf::Time update(sf::Time elapsedTime);

    template<typename TaskClass, typename ...Args>
    TaskHandle addTask(Args&&... args) {
        TaskHandle currentHandle = nextTaskHandle++;
        tasks[currentHandle] = new TaskClass(engine, logger, config, profiler, std::forward<Args>(args)...);
        return currentHandle;
    }

    template<typename TaskClass>
    TaskClass* getTask(TaskHandle handle) {
        return (TaskClass*) tasks[handle];
    }

    void deleteTask(TaskHandle task);

    TaskManager(Engine& engine, Logger& logger, Configuration& config, Profiler& profiler);
    ~TaskManager();

private:
    std::unordered_map<TaskHandle, Task*> tasks;
    TaskHandle nextTaskHandle = 1;

    Engine& engine;
    Logger& logger;
    Configuration& config;
    Profiler& profiler;
};
