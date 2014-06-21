#include "taskManager.h"
#include <SFML/System.hpp>
#include "common/emath.h"
#include "task.h"

sf::Time TaskManager::update(sf::Time elapsedTime) {
    engine.profiler.start("frame");
    sf::Time nextTaskUpdate{sf::seconds(std::numeric_limits<float>::max())};
    sf::Clock timeSinceNextTaskUpdate;

    for (auto& task : tasks) {
        task.second->accumulatedTime += elapsedTime;
        task.second->accumulatedTime = clamp(task.second->accumulatedTime, sf::milliseconds(0), sf::milliseconds(1000));

        while (task.second->accumulatedTime >= task.second->frequency) {
            task.second->update();
            task.second->accumulatedTime -= task.second->frequency;
        }

        if((nextTaskUpdate - timeSinceNextTaskUpdate.getElapsedTime()).asMicroseconds() > (task.second->frequency - task.second->accumulatedTime).asMicroseconds()) {
            timeSinceNextTaskUpdate.restart();
            nextTaskUpdate = (task.second->frequency - task.second->accumulatedTime);
        }
    }
    engine.profiler.stop();
    return (nextTaskUpdate - timeSinceNextTaskUpdate.getElapsedTime());
}

void TaskManager::deleteTask(TaskHandle task) {
    delete tasks[task];
    tasks.erase(task);
}

TaskManager::TaskManager(Engine& engine) : engine(engine) {
}

TaskManager::~TaskManager() {
    for (auto& task: tasks) {
        delete task.second;
    }
}
