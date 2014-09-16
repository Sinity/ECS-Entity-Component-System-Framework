#pragma once
#include <vector>
#include <chrono>

class Engine;
class Task;
/** \brief Manages all Tasks in the system
*
*  It is more flexible version of traditional game loop.
*  It uses fixed timestep approach.
*  Any Task can have different frequency - so, for example, physics can be 100Hz, rendering 30Hz, and ai 2Hz.
*/
class TaskManager {
public:
	TaskManager(Engine& engine);
	~TaskManager();

	/** \brief creates new task and adds it to system
	*
	* \param args arguments to be passed to task constructor
	*
	* \returns pointer to created Task.
	*/
	template<typename TaskClass, typename ...Args>
	TaskClass* addTask(Args&& ... args) {
		TaskClass* newTask = new TaskClass(engine, std::forward<Args>(args)...);
		tasks.push_back(newTask);
		return newTask;
	}

	/** deletes Task from the system
	*
	*   \param task pointer to task that will be deleted. nullptr allowed.
	*/
	void deleteTask(Task* task);

	/** \brief call to all Tasks that waits for it
	*
	*   \param elapsedTime time that has passed since last call of this method
	*
	*   \returns amount of time when it doesn't need to be called.
	*/
	std::chrono::milliseconds update(std::chrono::milliseconds elapsedTime);

private:
	std::vector<Task*> tasks;
	Engine& engine;
};
