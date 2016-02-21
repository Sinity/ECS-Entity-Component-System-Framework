#pragma once
#include "../utils/config.h"
#include "componentManager.h"
#include "entityManager.h"
#include "taskScheduler.h"
#include "eventQueue.h"

namespace EECS {
/** class that encapsulates whole ECS
*
* It ties all components together and manages it's configuration.
* It measures delta time for TaskScheduler.
*/
class ECS {
   public:
    ECS(const std::string& configFilename = "");

    // Runs main loop. Calls TaskScheduler::update periodically, feeding it with delta time.
    void run();

    // Will stop main loop at the next iteration.
    void stop();

    ComponentManager components;
    EntityManager entities;
    TaskScheduler tasks;
    EventQueue events;

    Configuration config;

   private:
    bool quit = false;
};
}
