#include <catch.hpp>
#include "ecs/ecs.h"
using namespace EECS;

struct FooComponent : public Component<FooComponent> {
    explicit FooComponent(int p = 0) : foo(p) {}

    int foo = 0;
};

TEST_CASE("Entity class works") {
    ComponentManager components;
    EntityManager entities{components};

    Entity entity{entities.addEntity(), entities, components};

    // this entity was just created, so it exists
    REQUIRE(entity.exists());

    // add some component
    entity.addComponent<FooComponent>(123);

    // now we can get it
    REQUIRE(entity.component<FooComponent>()->foo == 123);

    // test whether cloning works
    auto clone = entity.clone();
    REQUIRE(clone.component<FooComponent>()->foo == 123);

    // destroy a clone
    clone.destroy();
    REQUIRE_FALSE(clone.exists());

    // let's assign it to a valid entity
    clone.reassign(entity);

    // now both Entity instances point to the same entity
    REQUIRE(clone.getID() == entity.getID());

    // original FooComponent is still there
    REQUIRE(entity.component<FooComponent>());

    // we can also get handle to it
    auto handle = entity.componentHandle<FooComponent>();
    REQUIRE(handle);

    // delete it
    entity.deleteComponent<FooComponent>();
    REQUIRE_FALSE(entity.component<FooComponent>());
}
