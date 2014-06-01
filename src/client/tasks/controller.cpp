#include "controller.h"

#include "events/system/applicationClosed.h"

#include "tasks/renderer.h"
#include "tasks/input.h"
#include "tasks/debug.h"

void Controller::update() {
    profiler.start("Controller update");

    profiler.stop();
}

void Controller::receive(const ApplicationClosedEvent& UNUSED(closeRequest)) {
    logger.info("Application close request received in Controller, stopping engine...");
    engine.stop();
}

Controller::Controller(Engine& engine, Logger& logger, Configuration& config, Profiler& profiler) : Task(engine, logger, config, profiler) {
    engine.events.connect<ApplicationClosedEvent>(*this);

    engine.tasks.addTask<RenderingTask>(window);
    engine.tasks.addTask<InputTask>(window);
    engine.tasks.addTask<DebugTask>();
}

