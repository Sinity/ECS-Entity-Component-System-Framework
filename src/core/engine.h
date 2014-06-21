#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "componentContainer.h"
#include "taskManager.h"
#include "eventQueue.h"
#include "tool/profiler.h"
#include "tool/logger.h"

using TaskHandle = unsigned int;

class Engine {
public:
    Engine();

    bool init();
    void run();
    void stop();

    Logger logger;
    Profiler profiler;
    Configuration config;

    ComponentContainer components;
    TaskManager tasks;
    EventQueue events;

private:
    bool quit = false;
};
