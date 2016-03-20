#pragma once
#include <memory>
#include <vector>
#include <chrono>

namespace EECS {
class ECS;
class TaskBase;

/** \brief Manages all Tasks in the system
*
*  It is more flexible version of traditional game loop.
*  It uses fixed timestep approach.
*  Any Task can have different frequency - so, for example, physics can be 100Hz, rendering 30Hz, and ai 2Hz.
*/
class TaskScheduler {
   public:
    TaskScheduler(ECS& engine);
    ~TaskScheduler();
    void clear();

    template <typename TaskClass>
    TaskClass* getTask() {
        return (TaskClass*)tasks[TaskID::get<TaskClass>()].get();
    }

    /** \brief creates new task and adds it to system
    *
    * \param args arguments to be passed to task constructor
    *
    * \returns pointer to created Task.
    */
    template <typename TaskClass, typename... Args>
    TaskClass* addTask(Args&&... args) {
        auto task = std::make_unique<TaskClass>(engine, std::forward<Args>(args)...);
        tasks[TaskID::get<TaskClass>()] = std::move(task);
        return (TaskClass*)tasks[TaskID::get<TaskClass>()].get();
    }

    /** deletes Task from the system */
    template <typename TaskClass>
    void deleteTask() {
        tasks[TaskID::get<TaskClass>()].reset();
    }

    /** \brief call update method of all Tasks that wait for it
    *
    *   \param elapsedTime time that has passed since last call of this method
    *
    *   \returns amount of time when it doesn't need to be called again(interval to time when any task needs update)
    */
    std::chrono::milliseconds update(std::chrono::milliseconds elapsedTime);

   private:
    std::vector<std::unique_ptr<TaskBase>> tasks;
    ECS& engine;

    class TaskID {
       public:
        template <typename T>
        static size_t get() {
            static size_t id = counter++;
            return id;
        }

        static size_t count() { return counter; }

       private:
        static size_t counter;
    };
};
}
