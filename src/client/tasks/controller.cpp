#include "controller.h"

#include "events/system/applicationClosed.h"

#include "tasks/renderer.h"
#include "tasks/input.h"
#include "tasks/debug.h"

void Controller::update() {
    engine.profiler.start("Controller update");

    engine.profiler.stop();
}

void Controller::receive(const ApplicationClosedEvent& closeRequest) {
    engine.logger.info("Application close request received in Controller, stopping engine...");
    engine.stop();
}

Controller::Controller(Engine& engine) : Task(engine) {
    engine.events.connect<ApplicationClosedEvent>(*this);

    engine.tasks.addTask<RenderingTask>(window);
    engine.tasks.addTask<InputTask>(window);
    engine.tasks.addTask<DebugTask>();
}

