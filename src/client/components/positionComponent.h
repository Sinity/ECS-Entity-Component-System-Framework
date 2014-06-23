#pragma once
#include "core/component.h"
#include <SFML/System.hpp>

COMPONENT(PositionComponent) {
    PositionComponent(Entity owner, ComponentHandle handle) :
            Component(owner, handle) {
    }

    sf::Vector2f position = {0.0f, 0.0f};
};
