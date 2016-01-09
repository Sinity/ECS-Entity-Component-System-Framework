#include "ecs.h"
#include <thread>
#include "utils/loggerConsoleOutput.h"
#include "utils/loggerFileOutput.h"
#include "utils/timer.h"

using namespace EECS;

EECS::ECS::ECS(const std::string& configFilename) : logger("Main"), entities(components), tasks(*this) {
    components.setEntityManager(entities);

    if (!configFilename.empty()) {
        config.load(configFilename);
    }
}

void EECS::ECS::run() {
    Timer timer;
    std::chrono::milliseconds elapsedTime{0};

    while (!quit) {
        events.emit();
        auto durationSinceNextUpdateNecessary = tasks.update(elapsedTime);
        std::this_thread::sleep_for(durationSinceNextUpdateNecessary);
        elapsedTime = std::max(std::chrono::milliseconds(0), timer.reset());
    }
}

void EECS::ECS::stop() {
    quit = true;
}

