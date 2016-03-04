#include "ecs.h"
#include <thread>
#include "utils/timer.h"

using namespace EECS;

EECS::ECS::ECS(const std::string& configFilename) : entities(components), tasks(*this) {
    components.setEntityManager(entities);

    if (!configFilename.empty()) {
        config.load(configFilename);
    }
}

void EECS::ECS::run() {
    Timer timer;
    std::chrono::milliseconds elapsedTime{0};

    while (!quit) {
        auto durationUntilNextUpdateNecessary = tasks.update(elapsedTime);
        auto timeSpentSinceLastUpdate = Timer{};

        events.emit();

        std::this_thread::sleep_for(durationUntilNextUpdateNecessary - timeSpentSinceLastUpdate.elapsed());
        elapsedTime = std::max(std::chrono::milliseconds(0), timer.reset());
    }
}

void EECS::ECS::stop() { quit = true; }
