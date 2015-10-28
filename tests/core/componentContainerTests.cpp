#include <catch.hpp>
#include "include/ecs/ecs.h"

COMPONENT(AComponent) {
    AComponent(int init = 0)
            : foo(init) {
    }

    int foo = 0;
};

TEST_CASE("Adding component to null entity is impossible and yields nullptr") {
    auto comps = ComponentContainer<AComponent>{};
    auto componentPtr = comps.addComponent(0);

    //yields nullptr
    REQUIRE(componentPtr == nullptr);

    //not added to the system
    REQUIRE(comps.getComponent(0) == nullptr);
}

TEST_CASE("Getting component from wrong entity or which doesn't have this component at this moment gives nullptr") {
    auto comps = ComponentContainer<AComponent>{};

    //there were never such objects here
    REQUIRE(comps.getComponent(2) == nullptr);
    REQUIRE(comps.getComponent(1) == nullptr);

    comps.addComponent(1);

    //Entity 1 currently have AComponent
    REQUIRE(comps.getComponent(1) != nullptr);

    //Entity 1 have added an object, but 2 not, it's still not here
    REQUIRE(comps.getComponent(2) == nullptr);

    //After deletion, entity 1 doesn't have AComponent, so it should yield nullptr
    comps.deleteComponent(1);
    REQUIRE(comps.getComponent(1) == nullptr);
}

TEST_CASE("Getting component from right entity which have said component gives proper pointer") {
    auto comps = ComponentContainer<AComponent>{};

    auto p1 = comps.addComponent(1, 666);
    auto p2 = comps.addComponent(2, 333);

    //both objects successfully created
    REQUIRE((p1 && p2));

    //get valid pointers from the system - add/delete call could've caused previous pointers to turn invalid
    auto retrievedP1 = comps.getComponent(1);
    auto retrievedP2 = comps.getComponent(2);

    //sucessfully acquired pointers to the objects
    REQUIRE((retrievedP1 && retrievedP2));

    //content of objects is valid
    REQUIRE(retrievedP1->foo == 666);
    REQUIRE(retrievedP2->foo == 333);
}

TEST_CASE("Getting component after it was deleted gives nullptr, and doesn't affect other components") {
    auto comps = ComponentContainer<AComponent>{};
    comps.addComponent(1, 111);
    comps.addComponent(2, 222);
    comps.addComponent(3, 333);

    comps.deleteComponent(1);
    comps.deleteComponent(3);

    //components were deleted
    REQUIRE(comps.getComponent(1) == nullptr);
    REQUIRE(comps.getComponent(3) == nullptr);

    //remaining object stays the same
    REQUIRE(comps.getComponent(2) != nullptr);
    REQUIRE(comps.getComponent(2)->foo == 222);
}

TEST_CASE("Generic methods test") {
    auto comps = ComponentContainer<AComponent>{};
    auto genCompsHandle = (ComponentContainerBase*)&comps;

    auto component = (AComponent*)genCompsHandle->genericAddComponent(1);

    //component successfully added
    REQUIRE(component);

    component->foo = 123;

    //component successfully retrieved
    auto componentHandleFromSystem = (AComponent*)genCompsHandle->genericGetComponent(1);
    REQUIRE(componentHandleFromSystem->foo == 123);

    genCompsHandle->genericDeleteComponent(1);

    //component successfully deleted
    REQUIRE(comps.getComponent(1) == nullptr);

    //clear method test
    REQUIRE(genCompsHandle->genericAddComponent(1));
    genCompsHandle->clear();
    REQUIRE(genCompsHandle->genericGetComponent(1) == nullptr);
}

