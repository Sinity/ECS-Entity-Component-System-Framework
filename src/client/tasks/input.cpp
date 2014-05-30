#include "input.h"
#include "events/applicationClosed.h"
#include "events/keyPressed.h"
#include "events/keyReleased.h"
#include "events/mouseButtonPressed.h"
#include "events/mouseButtonReleased.h"
#include "events/mouseMoved.h"
#include "events/textEntered.h"
#include "events/unknownSFMLEvent.h"

InputTask::InputTask(Engine& engine, Logger& logger, Configuration& config, Profiler& profiler, sf::RenderWindow& window) :
    Task(engine, logger, config, profiler), window(window) {
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
