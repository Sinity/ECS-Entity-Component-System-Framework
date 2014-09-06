#pragma once

#include <boost/serialization/strong_typedef.hpp>
#include <unordered_map>

using Entity = unsigned int;
using ArgsMap = std::unordered_map<std::string, std::string>;

struct Component {
	Component(Entity owner) :
			owner(owner) {
	}

	virtual ~Component() {
	}

	virtual void init(ArgsMap args = ArgsMap()) {
		(void)args;
	};

	Entity owner;
};
