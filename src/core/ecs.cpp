#include "ecs.h"
#include <thread>
#include "utils/loggerConsoleOutput.h"
#include "utils/loggerFileOutput.h"
#include "utils/timer.h"

ECS::ECS(const std::string& configFilename) : logger("Main"), entities(components), tasks(*this) {
    components.setEntityManager(entities);

    if (!configFilename.empty()) {
        config.load(configFilename);
    }
}

void ECS::run() {
    Timer timer;
    std::chrono::milliseconds elapsedTime{0};

    while (!quit) {
        events.emit();
        auto durationSinceNextUpdateNecessary = tasks.update(elapsedTime);
        std::this_thread::sleep_for(durationSinceNextUpdateNecessary);
        elapsedTime = std::max(std::chrono::milliseconds(0), timer.reset());
    }
}

void ECS::stop() {
    quit = true;
}

