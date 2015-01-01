#pragma once
#include <chrono>
#include "engine.h"
#include "utils/config.h"

/** \brief implements independient portion of code, that is executed with some frequency
*
*   Tasks are usually called Systems in Entity-Component-Systems. I think Task is better name.
*
*   It is intended to operate on some component type. Example of Task may be PhysicsIntegrator, which gets
*   PhysicalBodyComponent and PositionComponent(by intersection, for example),
*   then calculates new position(PositionComponent) and velocity(PhysicalBodyComponent).
*
*   Another example might be Renderer, which gets PositionComponent and SpriteComponent,
*   and then displays sprite on screen at desired Position.
*
*   But Tasks are flexible, so you can use it to do any thing that should be done periodically.
*
*   By default, frequency will be once per game loop iteration(in config, task.defaultTaskFrequency).
*/
class Task {
public:
	Task(Engine& engine) :
			engine(engine) {
		frequency = std::chrono::milliseconds(engine.config.get("task.defaultTaskFrequency", 16));
	};

	/** \brief called at given frequency, derived class must implement it */
	virtual void update() = 0;

	virtual ~Task() {
	}

public:
	std::chrono::milliseconds frequency;
	std::chrono::milliseconds accumulatedTime{0};

protected:
	Engine& engine;
};
