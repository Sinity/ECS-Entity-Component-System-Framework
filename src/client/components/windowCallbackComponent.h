#pragma once

#include "ecs/component.h"
#include "../events/system/mouseButtonPressed.h"
#include "../events/system/mouseButtonReleased.h"
#include "../events/system/mouseMoved.h"
#include <SFML/System.hpp>

COMPONENT(WindowCallbackComponent) {
	std::function<bool(const MouseButtonPressed&)> mousePressCallback;
	std::function<bool(const MouseButtonReleased&)> mouseReleaseCallback;
	std::function<bool(const MouseMoved&)> mouseMoveCallback;
};
