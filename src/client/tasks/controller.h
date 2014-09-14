#pragma once

#include <SFML/Graphics.hpp>
#include "ecs/task.h"
#include "ecs/engine.h"

struct ApplicationClosedEvent;

class Controller : public Task {
public:
	Controller(Engine& engine);

	void receive(const ApplicationClosedEvent& closeRequest);

	void update() override;

private:
	sf::RenderWindow window;
};
