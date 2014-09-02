#pragma once

#include "core/component.h"

COMPONENT(WindowTreeComponent) {
	using Component::Component;

	void init(ArgsMap args = ArgsMap()) {
		parent = boost::lexical_cast<unsigned int>(args["parent"]);
	}

	Entity parent;
	std::vector<Entity> childs;
};
