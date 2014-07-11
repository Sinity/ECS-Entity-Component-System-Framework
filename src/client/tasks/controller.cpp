#include "controller.h"

#include "events/system/applicationClosed.h"

#include "tasks/renderer.h"
#include "tasks/input.h"
#include "tasks/debug.h"
#include "tasks/GUITask.h"

void Controller::update() {
}

void Controller::receive(const ApplicationClosedEvent& closeRequest) {
	(void)closeRequest;
    engine.logger.info("Application close request received in Controller, stopping engine...");
    engine.stop();
}

Controller::Controller(Engine& engine) : Task(engine) {
    engine.events.connect<ApplicationClosedEvent>(*this);

    engine.tasks.addTask<RenderingTask>(window);
    engine.tasks.addTask<InputTask>(window);
    engine.tasks.addTask<DebugTask>();
	engine.tasks.addTask<GUITask>();
}
