#pragma once
#include "core/task.h"
#include "core/engine.h"

struct ApplicationClosedEvent;
class Controller : public Task {
public:
    Controller(Engine& engine, Logger& logger, Configuration& config, Profiler& profiler);

    void receive(const ApplicationClosedEvent& closeRequest);
    void update() override;

private:
    sf::RenderWindow window;
};
