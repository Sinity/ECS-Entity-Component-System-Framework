#pragma once

#include <SFML/Graphics.hpp>
#include "ecs/task.h"
#include "ecs/engine.h"

class RenderingTask : public Task {
public:
	RenderingTask(Engine& engine, sf::RenderWindow& window);

	void update() override;

private:
	sf::RenderWindow& window;

	RenderingTask& operator=(RenderingTask&) = delete;
};
