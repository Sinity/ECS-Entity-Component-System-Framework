#pragma once

#include "ecs/task.h"
#include "ecs/engine.h"
#include <SFML/Graphics.hpp>
#include "../components/positionComponent.h"
#include "../components/relativePositionComponent.h"
#include "../components/renderingComponent.h"
#include "../components/sizeComponent.h"
#include "../components/windowCallbackComponent.h"
#include "../components/windowTreeComponent.h"
#include "../events/system/keyPressed.h"
#include "../events/system/keyReleased.h"
#include "../events/system/mouseButtonPressed.h"
#include "../events/system/mouseButtonReleased.h"
#include "../events/system/mouseMoved.h"
#include "../events/system/textEntered.h"

class GUITask : public Task {
public:
	GUITask(Engine& engine) :
			Task(engine) {
		engine.events.connect<KeyPressed>(*this);
		engine.events.connect<KeyReleased>(*this);
		engine.events.connect<TextEntered>(*this);
		engine.events.connect<MouseButtonPressed>(*this);
		engine.events.connect<MouseButtonReleased>(*this);
		engine.events.connect<MouseMoved>(*this);

		Entity sample = engine.components.createEntity();
		engine.components.createComponent<PositionComponent>(sample, {{"x", "1.0"}, {"y", "1.0"}});
		auto& repr = *engine.components.createComponent<RenderingComponent>(sample);
		auto& size = *engine.components.createComponent<SizeComponent>(sample, {{"width", "2.0"}, {"height", "1.0"}});
		auto& callbacks = *engine.components.createComponent<WindowCallbackComponent>(sample);
		engine.components.createComponent<WindowTreeComponent>(sample, {{"parent", "0"}});

		callbacks.mousePressCallback = [&engine](const MouseButtonPressed& ev)->bool {
		    engine.logger.info("ev Received, pos={", ev.button.x, ", ", ev.button.y, "}");
		    return true;
		};

		std::shared_ptr<sf::RectangleShape> bg = std::make_shared<sf::RectangleShape>(sf::Vector2f{size.width, size.height});
		bg->setFillColor(sf::Color::Green);
		repr.drawablesList.emplace_back(std::move(bg));
	}

	void update() override {
	}

	void receive(const KeyPressed& keyPressed) {
		(void)keyPressed;
	}

	void receive(const KeyReleased& keyReleased) {
		(void)keyReleased;
	}

	void receive(const TextEntered& textEntered) {
		(void)textEntered;
	}

	void receive(const MouseButtonPressed& mouseButtonPressed) {
		(void)mouseButtonPressed;
	}

	void receive(const MouseButtonReleased& mouseButtonReleased) {
		(void)mouseButtonReleased;
	}

	void receive(const MouseMoved& mouseMoved) {
		(void)mouseMoved;
	}

private:
	int leastFocusPlane = 0; //focus of the window on the floor of the window stack. Other windows will have lower plane.
	std::vector<Entity> windows;
};

