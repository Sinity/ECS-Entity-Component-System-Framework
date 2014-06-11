#include "core/event.h"
#include <SFML/Window.hpp>

struct TextEntered {
    static const EventType type = EventType::TextEntered;
    TextEntered(sf::Event::TextEvent text) : text(text) {}

    sf::Event::TextEvent text;
};

