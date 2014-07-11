#pragma once
#include "core/component.h"
#include <SFML/System.hpp>

COMPONENT(RelativePositionComponent) {
    using Component::Component;

	void init(ArgsMap args) {
		x = boost::lexical_cast<float>(args["x"]);
		y = boost::lexical_cast<float>(args["y"]);
	}

	float x = 0.f;
	float y = 0.f;
};
