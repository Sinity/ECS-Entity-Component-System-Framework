#include "core/event.h"
#include <SFML/Window.hpp>

struct ApplicationClosedEvent {
    static const EventType type = EventType::ApplicationClosed;
};

