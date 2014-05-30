#include "core/event.h"
#include <SFML/Window.hpp>

struct MouseButtonReleased {
    static const EventType type = EventType::MouseButtonReleased;
    MouseButtonReleased(sf::Event::MouseButtonEvent button) : button(button) {}

    sf::Event::MouseButtonEvent button;
};

