#pragma once
#include "core/component.h"
#include <SFML/System.hpp>

struct PositionComponent : public Component {
    static const ComponentType type = ComponentType::PositionComponent;

    PositionComponent(Entity owner, ComponentHandle handle, sf::Vector2f position = {0.0f, 0.0f}) :
            Component(owner, handle), position(position) {
    }

    sf::Vector2f position = {0.0f, 0.0f};
};
