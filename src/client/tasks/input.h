#pragma once
#include <SFML/Graphics.hpp>
#include "core/task.h"
#include "core/engine.h"

class InputTask : public Task {
public:
    InputTask(Engine& engine, Logger& logger, Configuration& config, Profiler& profiler, sf::RenderWindow& window);

    void update() override;

private:
    sf::RenderWindow& window;
};
