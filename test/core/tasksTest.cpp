#define BOOST_TEST_MODULE Tasks
#include <boost/test/unit_test.hpp>
#include "core/taskManager.h"
#include "core/task.h"

struct SampleComponent : public Component {
    SampleComponent(Entity owner, ComponentHandle handle) :
            Component(owner, handle) {
    }

    int content = 0;
};

struct SampleTask : public Task {
    SampleTask(Engine& engine, int step) : Task(engine), step(step) {
        this->frequency = sf::milliseconds(10);
    }

    void update() override {
        Components<SampleComponent> components = engine.components.getComponents<SampleComponent>();

        for (unsigned int i = 0; i < components.size(); i++) {
            components[i].content += step;
        }
    }

private:
    int step;
};

BOOST_AUTO_TEST_CASE(tasks_general_usage) {
	Engine engine;
	const int initialValue = 666;
	const int frequency = 10;
	const int step = 100;

	TaskHandle task = engine.tasks.addTask<SampleTask>(step);

	SampleComponent& component = engine.components.createComponent<SampleComponent>(engine.components.createEntity());
	component.content = initialValue;
	BOOST_CHECK_EQUAL(component.content, initialValue);

	engine.tasks.update(sf::milliseconds(frequency * 10));
	BOOST_CHECK_EQUAL(component.content, initialValue + step * 10);

	engine.tasks.update(sf::milliseconds(frequency - 1));
	BOOST_CHECK_EQUAL(component.content, initialValue + step * 10);
	BOOST_CHECK_EQUAL(engine.tasks.getTask<SampleTask>(task)->accumulatedTime.asMicroseconds(), sf::milliseconds(frequency - 1).asMicroseconds());

	engine.tasks.update(sf::milliseconds(1));
	BOOST_CHECK_EQUAL(component.content, initialValue + step * 11);
	BOOST_CHECK_EQUAL(engine.tasks.getTask<SampleTask>(task)->accumulatedTime.asMicroseconds(), sf::Time::Zero.asMicroseconds());
}
