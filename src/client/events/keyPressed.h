#include "core/event.h"
#include <SFML/Window.hpp>

struct KeyPressed {
    static const EventType type = EventType::KeyPressed;
    KeyPressed(sf::Event::KeyEvent key) : key(key) {}

    sf::Event::KeyEvent key;
};

