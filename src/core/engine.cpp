#include "engine.h"
#include "tool/loggerConsoleOutput.h"
#include "tool/loggerFileOutput.h"

bool Engine::init() {
	if(!config.load("config.cfg")) {
		return false;
	}
	if(!entityFactory.loadEntities("entities.cfg")) {
		return false;
	}

	components.configure(config.get<unsigned int>("componentContainer.maxComponentTypes", 4096),
	                     config.get<unsigned int>("componentContainer.growFactor", 16),
	                     config.get<unsigned int>("componentContainer.initialCapacity", 4096));
	events.configure(config.get<unsigned int>("eventQueue.maxEventTypes", 8192));
	return true;
}

void Engine::run() {
	sf::Clock clock;
	sf::Time elapsedTime = clock.restart();

	while(!quit) {
		events.emit();
		sf::Time nextUpdate = tasks.update(elapsedTime);
		sf::sleep(nextUpdate); //sleep 'till next update time
		elapsedTime = std::max(sf::Time::Zero, clock.restart());
	}
}

Engine::Engine() :
		logger("Main"),
		entityFactory(components),
		tasks(*this) {
	initLoggers("logz/main", true);
}

void Engine::stop() {
	quit = true;
}

void Engine::initLoggers(const std::string& filename, bool appendTimestamps) {
	logger.clearOutputs();
	components.logger.clearOutputs();
	config.logger.clearOutputs();

	std::shared_ptr<LoggerOutput> cOut = std::make_shared<ConsoleOutput>();
	cOut->setMinPriority(LogType::Error);
	logger.addOutput(std::move(cOut));

	std::shared_ptr<LoggerOutput> fOut = std::make_shared<FileOutput>(filename, appendTimestamps);
	logger.addOutput(std::move(fOut));

	components.logger.setOutputs(logger);
	config.logger.setOutputs(logger);
}
