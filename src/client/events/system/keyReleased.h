#include "core/event.h"
#include <SFML/Window.hpp>

struct KeyReleased {
    static const EventType type = EventType::KeyReleased;
    KeyReleased(sf::Event::KeyEvent key) : key(key) {}

    sf::Event::KeyEvent key;
};

