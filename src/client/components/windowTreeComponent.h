#pragma once
#include <boost/lexical_cast.hpp>
#include "core/component.h"

COMPONENT(WindowTreeComponent) {
	void init(ArgsMap args = ArgsMap()) {
		parent = boost::lexical_cast<unsigned int>(args["parent"]);
	}

	Entity parent;
	std::vector<Entity> childs;
};
