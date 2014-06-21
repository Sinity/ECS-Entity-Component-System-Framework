#pragma once
#include <unordered_map>
#include <SFML/System.hpp>

using TaskHandle = unsigned int;

class Engine;
class Task;
class TaskManager {
public:
	TaskManager(Engine& engine);
	~TaskManager();


	template<typename TaskClass>
	TaskClass* getTask(TaskHandle handle) {
		return (TaskClass*)tasks[handle];
	}

    template<typename TaskClass, typename ...Args>
    TaskHandle addTask(Args&&... args) {
        TaskHandle currentHandle = nextTaskHandle++;
        tasks[currentHandle] = new TaskClass(engine, std::forward<Args>(args)...);
        return currentHandle;
    }

    void deleteTask(TaskHandle task);
	sf::Time update(sf::Time elapsedTime);

private:
    std::unordered_map<TaskHandle, Task*> tasks;
    TaskHandle nextTaskHandle = 1;

    Engine& engine;

	TaskManager& operator=(TaskManager& taskManager) = delete;
};
