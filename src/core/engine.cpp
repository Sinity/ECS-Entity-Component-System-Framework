#include "ecs/engine.h"
#include <thread>
#include "ecs/utils/loggerConsoleOutput.h"
#include "ecs/utils/loggerFileOutput.h"
#include "../utils/timer.h"

bool Engine::init(const std::string& configFilename,
                  const std::string& entitiesDefinitionsFilename) {
	if(!config.load(configFilename)) {
		return false;
	}
	if(!entityFactory.loadEntities(entitiesDefinitionsFilename)) {
		return false;
	}

	components.configure(config.get<unsigned int>("componentContainer.maxComponentTypes", 4096),
	                     config.get<unsigned int>("componentContainer.growFactor", 16),
	                     config.get<unsigned int>("componentContainer.initialCapacity", 4096));
	events.configure(config.get<unsigned int>("eventQueue.maxEventTypes", 8192));
	return true;
}

void Engine::run() {
	Timer timer;
	std::chrono::milliseconds elapsedTime{0};

	while(!quit) {
		events.emit();
		std::chrono::milliseconds durationSinceNextUpdateNecessary = tasks.update(elapsedTime);
		std::this_thread::sleep_for(durationSinceNextUpdateNecessary);
		elapsedTime = std::max(std::chrono::milliseconds(0), timer.reset());
	}
}

Engine::Engine() :
		logger("Main"),
		entityFactory(components),
		tasks(*this) {
	initLoggers(config.get<std::string>("engine.loggerPath", "logz/engine"), true);
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
