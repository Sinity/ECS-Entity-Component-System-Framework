#include <catch.hpp>
#include "include/ecs/ecs.h"

COMPONENT(FooComponent) {
    explicit FooComponent(int p = 0) : foo(p) {
    }

    int foo = 0;
};

COMPONENT(BarComponent) {
    explicit BarComponent(int p = 0) : bar(p) {
    }

    int bar = 0;
};

TEST_CASE("Basic methods test") {
    ComponentManager comps;

    auto comp = comps.addComponent("FooComponent", 1);
    auto comp2 = comps.addComponent<BarComponent>(1);

    //components successfully added
    REQUIRE((comp && comp2));

    //components accessible from the system
    REQUIRE(comps.getComponent<BarComponent>(1));
    REQUIRE(comps.getComponent("FooComponent", 1));

    //Can delete components
    comps.deleteComponent<FooComponent>(1);
    comps.deleteComponent("BarComponent", 1);

    //So, now they aren't here anymore
    REQUIRE(!comps.getComponent<BarComponent>(1));
    REQUIRE(!comps.getComponent("FooComponent", 1));
}

TEST_CASE("validComponentPointer test") {
    ComponentManager comps;

    auto comp = comps.addComponent<FooComponent>(1);

    //As no operations were done after adding this component, it's pointers are valid.
    REQUIRE(comps.validComponentPointer(comp, 1));

    //It's pointer to component of entity 1, not 2
    REQUIRE(!comps.validComponentPointer(comp, 2));

    //Testing off-by-one pointer
    REQUIRE(!comps.validComponentPointer(comp + 1, 1));

    comps.deleteComponent<FooComponent>(1);

    //Now, as it's deleted, pointer to it is invalid
    REQUIRE(!comps.validComponentPointer(comp, 1));
}

TEST_CASE("Testing checkings if entity exists") {
    ComponentManager comps;
    EntityManager entities(comps);

    //before checking turned on, we can add components to entites which doesn't exist
    REQUIRE(!entities.entityExists(666));
    REQUIRE(comps.addComponent<FooComponent>(666));

    //enabling checking
    comps.setEntityManager(entities);

    //can't add component to entity which doesn't exist
    REQUIRE(!entities.entityExists(123));
    REQUIRE(!comps.addComponent<FooComponent>(123));

    //can add component to entity which exists
    auto entity = entities.addEntity();
    REQUIRE(comps.addComponent<FooComponent>(entity));
}

TEST_CASE("Intersection method test") {
    ComponentManager comps;

    //make 2 entites which have both components
    comps.addComponent<FooComponent>(1, 11);
    comps.addComponent<BarComponent>(1, 12);
    comps.addComponent<FooComponent>(2, 21);
    comps.addComponent<BarComponent>(2, 22);

    //make entity which has only FooComponent, and another enitity which has only BarComponent
    comps.addComponent<FooComponent>(3, 31);
    comps.addComponent<BarComponent>(4, 42);

    //content of components is 2digit number, first digit is EntityID and second is type(Foo - 1, Bar - 2)

    auto both = comps.intersection<FooComponent, BarComponent>();

    //2 entites meet criteria
    REQUIRE(both.size() == 2);

    //NOTE: it's not requirement that older entities have smaller indices, so it's a bit implementation-specific.

    //Check components content
    REQUIRE(both[0].get<FooComponent>().foo == 11);
    REQUIRE(both[1].get<FooComponent>().foo == 21);
    REQUIRE(both[0].get<BarComponent>().bar == 12);
    REQUIRE(both[1].get<BarComponent>().bar == 22);

    //Check if we can modify accessed components
    both[0].get<FooComponent>().foo = 666;
    REQUIRE(both[0].get<FooComponent>().foo == 666);
    REQUIRE(comps.getComponent<FooComponent>(1)->foo == 666);
}
