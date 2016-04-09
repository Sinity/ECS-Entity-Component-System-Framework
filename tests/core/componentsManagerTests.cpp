#include <catch.hpp>
#include "include/ecs/ecs.h"
using namespace EECS;

struct FooComponent : public Component<FooComponent> {
    explicit FooComponent(int p = 0) : foo(p) {}

    int foo = 0;
};

struct BarComponent : public Component<BarComponent> {
    explicit BarComponent(int p = 0) : bar(p) {}

    int bar = 0;
};

TEST_CASE("Basic methods test") {
    ComponentManager comps;

    auto comp = comps.addComponent<BarComponent>(1);

    // component successfully added
    REQUIRE(comp);

    // component accessible from the system
    REQUIRE(comps.getComponent<BarComponent>(1));

    // Can delete component
    comps.deleteComponent<BarComponent>(1);

    // So, now it's not here anymore
    REQUIRE(!comps.getComponent<BarComponent>(1));
}

TEST_CASE("validComponentPointer test") {
    ComponentManager comps;

    auto comp = (FooComponent*)comps.addComponent<FooComponent>(1);

    // As no operations were done after adding this component, it's pointers are valid.
    REQUIRE(comps.validComponentPointer(comp, 1));

    // It's pointer to component of entity 1, not 2
    REQUIRE(!comps.validComponentPointer(comp, 2));

    // Testing off-by-one pointer
    REQUIRE(!comps.validComponentPointer(comp + 1, 1));

    comps.deleteComponent<FooComponent>(1);

    // Now, as it's deleted, pointer to it is invalid
    REQUIRE(!comps.validComponentPointer(comp, 1));
}

TEST_CASE("Testing checks if entity exists") {
    ComponentManager comps;
    EntityManager entities(comps);

    // before checking turned on, we can add components to entities which doesn't exist
    REQUIRE(!entities.entityExists(666));
    REQUIRE(comps.addComponent<FooComponent>(666));

    // enabling checking
    comps.setEntityManager(entities);

    // can't add component to entity which doesn't exist
    REQUIRE(!entities.entityExists(123));
    REQUIRE(!comps.addComponent<FooComponent>(123));

    // can add component to entity which exists
    auto entity = entities.addEntity();
    REQUIRE(comps.addComponent<FooComponent>(entity));
}

TEST_CASE("Intersection method test") {
    ComponentManager comps;

    // make 2 entities which have both components
    comps.addComponent<FooComponent>(1, 11);
    comps.addComponent<BarComponent>(1, 12);
    comps.addComponent<FooComponent>(2, 21);
    comps.addComponent<BarComponent>(2, 22);

    // make entity which has only FooComponent, and another enitity which has only BarComponent
    comps.addComponent<FooComponent>(3, 31);
    comps.addComponent<BarComponent>(4, 42);

    // content of components is 2digit number, first digit is EntityID and second is type(Foo - 1, Bar - 2)

    auto both = comps.intersection<FooComponent, BarComponent>();

    // 2 entites meet criteria
    REQUIRE(both.size() == 2);

    // NOTE: it's not requirement that older entities have smaller indices, so it's a bit implementation-specific.

    // Check components content
    REQUIRE(both[0].get<FooComponent>().foo == 11);
    REQUIRE(both[1].get<FooComponent>().foo == 21);
    REQUIRE(both[0].get<BarComponent>().bar == 12);
    REQUIRE(both[1].get<BarComponent>().bar == 22);

    // Check if we can modify accessed components
    both[0].get<FooComponent>().foo = 666;
    REQUIRE(both[0].get<FooComponent>().foo == 666);
    REQUIRE(comps.getComponent<FooComponent>(1)->foo == 666);
}

TEST_CASE("Intersection method test - multithreading") {
    ComponentManager comps;

    for (auto i = 1; i <= 100; i++) {
        // make 100 entities with both types
        comps.addComponent<FooComponent>(i);
        comps.addComponent<BarComponent>(i);

        // and 200 entities with only one
        comps.addComponent<FooComponent>(i + 100);
        comps.addComponent<BarComponent>(i + 200);
    }

    auto intersection = comps.intersection<FooComponent, BarComponent>();

    // There are exactly 100 entities with both types of component.
    REQUIRE(intersection.size() == 100);
}
