#include "engine.h"
#include <SFML/System.hpp>

bool Engine::init() {
    if (!config.load("config.cfg"))
        return false;
    return true;
}

void Engine::run() {
    sf::Clock clock;
    sf::Time elapsedTime = clock.restart();

    while (!quit) {
		profiler.start("Main Loop");

		//update game
		events.emit();
		sf::Time nextUpdate = tasks.update(elapsedTime);

		//sleep 'till next update time
		profiler.start("sleep");
		sf::sleep(nextUpdate);
		profiler.stop();

		//calculate how much time passed from previous update
		elapsedTime = std::max(sf::Time::Zero, clock.restart());
		profiler.stop();
    }
}

Engine::Engine() :
    logger("Main"),
    profiler("profiler.txt"),
    components(config),
	tasks(*this) {
}

void Engine::stop() {
    quit = true;
}
