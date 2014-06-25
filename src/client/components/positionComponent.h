#pragma once
#include "core/component.h"
#include <SFML/System.hpp>

COMPONENT(PositionComponent) {
    PositionComponent(Entity owner, ComponentHandle handle) :
            Component(owner, handle) {
    }

	void init(ArgsMap args) {
		position.x = boost::lexical_cast<float>(args["x"]);
		position.y = boost::lexical_cast<float>(args["y"]);
	}

    sf::Vector2f position = {0.0f, 0.0f};
};
