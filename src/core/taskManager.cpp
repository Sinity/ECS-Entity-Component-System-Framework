#include "taskManager.h"
#include "common/emath.h"
#include "task.h"

sf::Time TaskManager::update(sf::Time elapsedTime) {
	sf::Time nextTaskUpdate{sf::seconds(std::numeric_limits<float>::max())};
	sf::Clock timeAlreadyElapsed;

	for(auto& task : tasks) {
		task->accumulatedTime += elapsedTime;
		task->accumulatedTime = clamp(task->accumulatedTime, sf::milliseconds(0), sf::milliseconds(1000));

		while(task->accumulatedTime >= task->frequency) {
			task->update();
			task->accumulatedTime -= task->frequency;
		}

		if(nextTaskUpdate - timeAlreadyElapsed.getElapsedTime() > task->frequency - task->accumulatedTime) {
			nextTaskUpdate = task->frequency - task->accumulatedTime;
			timeAlreadyElapsed.restart();
		}
	}

	return nextTaskUpdate - timeAlreadyElapsed.getElapsedTime();
}

void TaskManager::deleteTask(Task* task) {
	for(unsigned int i = 0; i < tasks.size(); i++) {
		if(tasks[i] == task) {
			delete tasks[i];
			tasks.erase(tasks.begin() + i);
		}
	}
}

TaskManager::TaskManager(Engine& engine) :
		engine(engine) {
}

TaskManager::~TaskManager() {
	for(auto task : tasks) {
		delete task;
	}
}
