#pragma once
#include "utils/logger.h"
#include "utils/config.h"
#include "core/componentContainer.h"
#include "core/entityFactory.h"
#include "core/taskManager.h"
#include "core/eventQueue.h"

/** class that encapsulates whole ECS
*
* It ties all components together and manages it's configuration.
* It measures delta time for TaskManager.
*/
class Engine {
public:
	Engine();

	/** \brief loads and setups engine
	*
	* \param configFilename name of file that contains main program configuration.
	* \param entitiesDefinitionsFilename name of file that containes entities definitions, for EntityFactory
	*
	* It will configure all parts with configuration.
	*/
	bool init(const std::string& configFilename, const std::string& entitiesDefinitionsFilename);

	/** \brief runs main loop(TaskManager)
	*
	* It calls TaskManager::update periodically, feeding it with delta time, until stop is called from somewhere.
	*/
	void run();

	/** \brief will stop main loop at the next iteration. */
	void stop();

	Logger logger;
	Configuration config;

	ComponentContainer components;
	EntityFactory entityFactory;
	TaskManager tasks;
	EventQueue events;

private:
	bool quit = false;
	void initLoggers(const std::string& filename, bool appendTimestamps);
};
