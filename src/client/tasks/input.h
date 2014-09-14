#pragma once

#include <SFML/Graphics.hpp>
#include "ecs/task.h"
#include "ecs/engine.h"

class InputTask : public Task {
public:
	InputTask(Engine& engine, sf::RenderWindow& window);

	void update() override;

private:
	sf::RenderWindow& window;
};
