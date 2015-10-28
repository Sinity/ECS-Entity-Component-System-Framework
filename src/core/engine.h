#pragma once
#include "../utils/logger.h"
#include "../utils/config.h"
#include "componentsManager.h"
#include "entityManager.h"
#include "taskManager.h"
#include "eventQueue.h"

/** class that encapsulates whole ECS
*
* It ties all components together and manages it's configuration.
* It measures delta time for TaskManager.
*/
class Engine {
public:
    Engine(const std::string& configFilename = "");

    //Runs main loop. Calls TaskManager::update periodically, feeding it with delta time.
    void run();

    // Will stop main loop at the next iteration.
    void stop();

    Logger logger;
    Configuration config;

    ComponentsManager components;
    EntityManager entities;
    TaskManager tasks;
    EventQueue events;

private:
    bool quit = false;
};
