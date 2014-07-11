#include "taskManager.h"
#include "task.h"
#include <SFML/System.hpp>
#include "tool/logger.h"
#include "common/emath.h"

sf::Time TaskManager::update(sf::Time elapsedTime) {
    sf::Time nextTaskUpdate{sf::seconds(std::numeric_limits<float>::max())};
    sf::Clock timeElapsedSinceLastTaskUpdate;

    for (auto& task : tasks) {
        task.second->accumulatedTime += elapsedTime;
        task.second->accumulatedTime = clamp(task.second->accumulatedTime, sf::milliseconds(0), sf::milliseconds(1000));

        while (task.second->accumulatedTime >= task.second->frequency) {
            task.second->update();
            task.second->accumulatedTime -= task.second->frequency;
        }

        if((nextTaskUpdate - timeElapsedSinceLastTaskUpdate.getElapsedTime()).asMicroseconds() > (task.second->frequency - task.second->accumulatedTime).asMicroseconds()) {
            timeElapsedSinceLastTaskUpdate.restart();
            nextTaskUpdate = (task.second->frequency - task.second->accumulatedTime);
        }
    }
    return (nextTaskUpdate - timeElapsedSinceLastTaskUpdate.getElapsedTime());
}

void TaskManager::deleteTask(TaskHandle task) {
    delete tasks[task];
    tasks.erase(task);
}

TaskManager::TaskManager(Engine& engine) :
	engine(engine) {
}

TaskManager::~TaskManager() {
    for (auto& task: tasks) {
        delete task.second;
    }
}
