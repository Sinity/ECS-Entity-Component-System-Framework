#pragma once
#include <boost/serialization/strong_typedef.hpp>
#include <unordered_map>

using ComponentHandle = unsigned long long;
BOOST_STRONG_TYPEDEF(unsigned int, Entity);

struct Component {
    Component(Entity owner, ComponentHandle handle) :
		owner(owner),
		handle(handle) {
	}

    virtual ~Component() {}
	virtual void init(std::unordered_map<std::string, std::string> args = {}) {};

    Entity owner;
    ComponentHandle handle;
};
