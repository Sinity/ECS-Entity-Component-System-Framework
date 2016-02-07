#include <catch.hpp>
#include "ecs/ecs.h"
using namespace EECS;

class TestTask : public Task {
   public:
    TestTask(ECS& engine) : Task(engine) {}

    void update() { updateCounter++; };

    size_t updateCounter = 0;
};

class OtherTestTask : public Task {
   public:
    OtherTestTask(ECS& engine) : Task(engine) {}

    void update() { updateCounter++; };

    size_t updateCounter = 0;
};

TEST_CASE("elapsedTime->0, even several times, won't update tasks", "[TaskScheduler]") {
    ECS engine;
    TaskScheduler taskManager(engine);

    auto firstTask = taskManager.addTask<TestTask>();
    auto secondTask = taskManager.addTask<OtherTestTask>();
    firstTask->frequency = std::chrono::milliseconds(1);
    secondTask->frequency = std::chrono::milliseconds(1);

    for (unsigned int i = 0; i < 100; i++) {
        taskManager.update(std::chrono::milliseconds(0));
    }

    REQUIRE(firstTask->updateCounter == 0);
    REQUIRE(secondTask->updateCounter == 0);
}

TEST_CASE("elapsedTime->taskFrequency - 1 won't update task. Second update with elapedTime->1 will do.") {
    ECS engine;
    TaskScheduler taskManager(engine);
    auto sampleTask = taskManager.addTask<TestTask>();
    sampleTask->frequency = std::chrono::milliseconds(42);

    taskManager.update(std::chrono::milliseconds(41));
    REQUIRE(sampleTask->updateCounter == 0);

    taskManager.update(std::chrono::milliseconds(1));
    REQUIRE(sampleTask->updateCounter == 1);
}

TEST_CASE(
    "Time below task frequency is accumulated, so several delta times below freq. will yield task update."
    "Small acumulation of time (< frequency) won't yield task update.") {
    ECS engine;
    TaskScheduler taskManager(engine);
    auto sampleTask = taskManager.addTask<TestTask>();
    sampleTask->frequency = std::chrono::milliseconds(100);

    for (unsigned int i = 0; i < 300; i++) {
        taskManager.update(std::chrono::milliseconds(1));
    }

    REQUIRE(sampleTask->updateCounter == 3);

    taskManager.update(std::chrono::milliseconds(1));
    REQUIRE(sampleTask->updateCounter == 3);
}

TEST_CASE("Two tasks with different frequencies", "[TaskScheduler]") {
    ECS engine;
    TaskScheduler taskManager(engine);
    auto firstTask = taskManager.addTask<TestTask>();
    auto secondTask = taskManager.addTask<OtherTestTask>();
    firstTask->frequency = std::chrono::milliseconds(10);
    secondTask->frequency = std::chrono::milliseconds(100);

    for (unsigned int i = 0; i < 200; i++) {
        taskManager.update(std::chrono::milliseconds(1));
    }

    REQUIRE(firstTask->updateCounter == 20);
    REQUIRE(secondTask->updateCounter == 2);
}

TEST_CASE("Time to next task update with single task returns approx. task freq - task accumulated time") {
    ECS engine;
    TaskScheduler taskManager(engine);
    auto sampleTask = taskManager.addTask<TestTask>();
    sampleTask->frequency = std::chrono::milliseconds(10);

    auto timeToNextUpdate = taskManager.update(std::chrono::milliseconds(3));

    // time that passed since last task update should be substracted, but std::chrono::milliseconds is integer so
    // this negligable(in this case) amount of time will be discarded anyway.
    REQUIRE(timeToNextUpdate.count() == 10 - 3);
}

TEST_CASE("Time to next task update with single task returns approx. tasks(min(task.freq - task.accumulatedTime))") {
    ECS engine;
    TaskScheduler taskManager(engine);
    auto firstTask = taskManager.addTask<TestTask>();
    auto secondtTask = taskManager.addTask<OtherTestTask>();
    firstTask->frequency = std::chrono::milliseconds(10);
    secondtTask->frequency = std::chrono::milliseconds(101);

    auto timeToNextUpdate = taskManager.update(std::chrono::milliseconds(100));

    // time that passed since last task update should be substracted, but std::chrono::milliseconds is integer so
    // this negligable(in this case) amount of time will be discarded anyway.
    REQUIRE(timeToNextUpdate.count() == 1);
}

TEST_CASE("Task retrieval and delete test") {
    ECS engine;
    TaskScheduler taskManager(engine);

    auto testTask = taskManager.addTask<TestTask>();
    REQUIRE(testTask);

    auto testTaskRetrieved = taskManager.getTask<TestTask>();
    REQUIRE(testTaskRetrieved == testTask);

    taskManager.deleteTask<TestTask>();
    REQUIRE(!taskManager.getTask<TestTask>());
}
