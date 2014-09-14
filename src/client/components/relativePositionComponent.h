#pragma once

#include "ecs/component.h"

COMPONENT(RelativePositionComponent) {
	void init(ArgsMap args) {
		x = boost::lexical_cast<float>(args["x"]);
		y = boost::lexical_cast<float>(args["y"]);
	}

	float x = 0.f;
	float y = 0.f;
};
