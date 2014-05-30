#include "core/engine.h"
#include "tasks/controller.h"

int main() {
    Engine engine;

    if (!engine.init()) {
        engine.logger.fatal("main: engine initialization failed. Stopping execution.");
        return EXIT_FAILURE;
    }

    engine.tasks.addTask<Controller>();

    engine.run();

    engine.logger.info("Application executed, returning to system");
    return 0;
}

