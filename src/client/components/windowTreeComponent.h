#pragma once
#include "core/component.h"

COMPONENT(WindowTreeComponent) {
	WindowTreeComponent(Entity owner, ComponentHandle handle) :
		Component(owner, handle) {}

	Entity parent;
	std::vector<Entity> childs;
};
