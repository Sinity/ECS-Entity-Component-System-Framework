#pragma once
#include "core/component.h"

COMPONENT(WindowTreeComponent) {
    using Component::Component;

	Entity parent;
	std::vector<Entity> childs;
};
