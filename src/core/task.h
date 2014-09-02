#pragma once

#include <SFML/System.hpp>
#include "engine.h"
#include "tool/config.h"

class Task {
public:
	Task(Engine& engine) :
			engine(engine) {
		frequency = sf::milliseconds(engine.config.get("task.defaultTaskFrequency", 16));
	}

	virtual void update() = 0;

	virtual ~Task() {
	}

public:
	sf::Time frequency;
	sf::Time accumulatedTime = sf::milliseconds(0);

protected:
	Engine& engine;

	Task& operator=(Task& task) = delete;
};

