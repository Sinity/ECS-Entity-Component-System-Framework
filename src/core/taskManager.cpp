#include "taskManager.h"
#include "common/emath.h"
#include "tool/timer.h"
#include "task.h"

std::chrono::milliseconds TaskManager::update(std::chrono::milliseconds elapsedTime) {
	std::chrono::milliseconds nextTaskUpdate{std::chrono::milliseconds::max()};
	Timer timeAlreadyElapsed;

	for(auto& task : tasks) {
		task->accumulatedTime += elapsedTime;
		task->accumulatedTime = clamp(task->accumulatedTime,
		                              std::chrono::milliseconds(0),
		                              std::chrono::milliseconds(1000));

		while(task->accumulatedTime >= task->frequency) {
			task->update();
			task->accumulatedTime -= task->frequency;
		}

		if(nextTaskUpdate - timeAlreadyElapsed.elapsed() > task->frequency - task->accumulatedTime) {
			nextTaskUpdate = task->frequency - task->accumulatedTime;
			timeAlreadyElapsed.reset();
		}
	}

	return nextTaskUpdate - timeAlreadyElapsed.elapsed();
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
