#include "core/event.h"
#include <SFML/Window.hpp>

struct MouseButtonPressed {
    static const EventType type = EventType::MouseButtonPressed;
    MouseButtonPressed(sf::Event::MouseButtonEvent button) : button(button) {}

    sf::Event::MouseButtonEvent button;
};

