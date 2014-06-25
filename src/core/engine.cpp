#include "engine.h"
#include <SFML/System.hpp>
#include "tool/loggerConsoleOutput.h"
#include "tool/loggerFileOutput.h"

bool Engine::init() {
    if (!config.load("config.cfg"))
        return false;
	if (!entityFactory.loadEntities("entities.cfg")) {
		return false;
	}

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
	entityFactory(components),
	tasks(*this) {
	
	initLoggers("logz/main", true);
}

void Engine::stop() {
    quit = true;
}

void Engine::initLoggers(const std::string& filename, bool appendTimestamps)
{
	logger.clearOutputs();
	profiler.logger.clearOutputs();
	components.logger.clearOutputs();
	config.logger.clearOutputs();

	std::shared_ptr<LoggerOutput> cOut = std::make_shared<ConsoleOutput>();
	cOut->setMinPriority(LogType::Error);
	logger.addOutput(std::move(cOut));

	std::shared_ptr<LoggerOutput> fOut = std::make_shared<FileOutput>(filename, appendTimestamps);
	logger.addOutput(std::move(fOut));

	profiler.logger.setOutputs(logger);
	components.logger.setOutputs(logger);
	config.logger.setOutputs(logger);
}
