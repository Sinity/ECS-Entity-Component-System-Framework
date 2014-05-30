#include "core/event.h"
#include <SFML/Window.hpp>
#include "common/unused.h"

struct ApplicationClosedEvent {
    static const EventType type = EventType::ApplicationClosed;
};

