#include "input.h"
#include "../events/system/applicationClosed.h"
#include "../events/system/keyPressed.h"
#include "../events/system/keyReleased.h"
#include "../events/system/mouseButtonPressed.h"
#include "../events/system/mouseButtonReleased.h"
#include "../events/system/mouseMoved.h"
#include "../events/system/textEntered.h"
#include "../events/system/unknownSFMLEvent.h"

InputTask::InputTask(Engine& engine, sf::RenderWindow& window) :
		Task(engine),
		window(window) {
	window.setKeyRepeatEnabled(false);
}

void InputTask::update() {
	sf::Event currentEvent;
	while(window.pollEvent(currentEvent)) {
		switch(currentEvent.type) {
			case sf::Event::Closed:
				engine.events.emplace<ApplicationClosedEvent>();
		        break;
			case sf::Event::KeyPressed:
				engine.events.emplace<KeyPressed>(currentEvent.key);
		        break;
			case sf::Event::KeyReleased:
				engine.events.emplace<KeyReleased>(currentEvent.key);
		        break;
			case sf::Event::TextEntered:
				engine.events.emplace<TextEntered>(currentEvent.text);
		        break;
			case sf::Event::MouseButtonPressed:
				engine.events.emplace<MouseButtonPressed>(currentEvent.mouseButton);
		        break;
			case sf::Event::MouseButtonReleased:
				engine.events.emplace<MouseButtonReleased>(currentEvent.mouseButton);
		        break;
			case sf::Event::MouseMoved:
				engine.events.emplace<MouseMoved>(currentEvent.mouseMove);
		        break;
			default:
				engine.events.emplace<UnknownSFMLEvent>(currentEvent);
		}
	}
}
