#pragma once
#include <boost/serialization/strong_typedef.hpp>

enum class ComponentType {
    SampleComponent, //for unit test
    AnotherSampleComponent, //for unit testing
    PositionComponent,
    GraphicsComponent,

	AmountOfComponentTypes
};

using ComponentHandle = unsigned long long;
BOOST_STRONG_TYPEDEF(unsigned int, Entity);

struct Component {
    Component(Entity owner, ComponentHandle handle) : owner(owner), handle(handle) {}
    virtual ~Component() {}

    Entity owner;
    ComponentHandle handle;
};
