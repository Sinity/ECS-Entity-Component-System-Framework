#include "engine.h"

bool Engine::init() {
    if (!config.load("config.cfg"))
        return false;

    srand(config.get("seed", (unsigned int)time(0)));

    return true;
}

void Engine::run() {
    sf::Clock clock;
    sf::Clock eventsHandlingTime;
    sf::Time elapsedTime = clock.restart();

    while (!quit) {
        sf::Time nextUpdate = tasks.update(elapsedTime);

        eventsHandlingTime.restart();
        events.emit();
        
        profiler.start("sleep");
        sf::sleep(nextUpdate - eventsHandlingTime.getElapsedTime());
        profiler.stop();

        elapsedTime = std::max(sf::Time::Zero, clock.restart());
    }
}

Engine::Engine() :
    logger("Main"),
    profiler("profiler.txt", logger),
    config(logger),
    components(logger, config),
    tasks(*this)
{}

void Engine::stop() {
    quit = true;
}
