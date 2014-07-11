#pragma once
#include "tool/logger.h"
#include "tool/config.h"
#include "componentContainer.h"
#include "entityFactory.h"
#include "taskManager.h"
#include "eventQueue.h"

class Engine {
public:
    Engine();
	
    bool init();
    void run();
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
