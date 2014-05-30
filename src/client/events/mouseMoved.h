#include "core/event.h"
#include <SFML/Window.hpp>

struct MouseMoved {
    static const EventType type = EventType::MouseMoved;
    MouseMoved(sf::Event::MouseMoveEvent where) : where(where) {}

    sf::Event::MouseMoveEvent where;
};

