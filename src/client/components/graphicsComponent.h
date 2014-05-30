#pragma once
#include "core/component.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct GraphicsComponent : public Component {
    static const ComponentType type = ComponentType::GraphicsComponent;

    GraphicsComponent(Entity owner, ComponentHandle handle, sf::Drawable* representation, int plane = 0) :
            Component(owner, handle), plane(plane), representation(representation) {
    }

    int plane;
    std::unique_ptr<sf::Drawable> representation;
};
