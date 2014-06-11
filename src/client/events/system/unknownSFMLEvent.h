#include "core/event.h"
#include <SFML/Window.hpp>

struct UnknownSFMLEvent {
    static const EventType type = EventType::UnknownSFMLEvent;
    UnknownSFMLEvent(sf::Event event) : event(event) { }

    sf::Event event;
};
