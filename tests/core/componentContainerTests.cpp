#include <catch.hpp>
#include "src/core/componentContainer.h"
#include "ecs/component.h"

COMPONENT(AComponent) {
};
COMPONENT(BComponent) {
};
COMPONENT(CComponent) {
};

TEST_CASE("first Entity isn't null Entity", "[ComponentContainer][createEntity]") {
	ComponentContainer componentContainer;
	REQUIRE(componentContainer.createEntity() != 0);
}

TEST_CASE("Created entity exists", "[ComponentContainer][entityExist][createEntity]") {
	ComponentContainer componentContainer;
	Entity createdEntity = componentContainer.createEntity();
	REQUIRE(componentContainer.entityExist(createdEntity));
}

TEST_CASE("any component of non-existing Entity don't exist", "[ComponentContainer][componentExist]") {
	ComponentContainer componentContainer;
	Entity toDelete = componentContainer.createEntity();
	componentContainer.deleteEntity(toDelete);
	REQUIRE(componentContainer.componentExist<AComponent>(toDelete) == false);
}

TEST_CASE("For Entity that has not component at given type, component of that type don't exist",
          "[ComponentContainer][componentExist]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	REQUIRE_FALSE(componentContainer.componentExist<AComponent>(sampleEntity));
}

TEST_CASE("If component was created, but for other Entity, then it don't exist for this Entity",
          "[ComponentContainer][componentExist]") {
	ComponentContainer componentContainer;
	Entity firstEntity = componentContainer.createEntity();
	Entity secondEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(firstEntity);
	REQUIRE_FALSE(componentContainer.componentExist<AComponent>(secondEntity));
}

TEST_CASE("If component was created for given Entity, but was delated, then it don't exist",
          "[ComponentContainer][componentExist]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(sampleEntity);
	componentContainer.deleteComponent<AComponent>(sampleEntity);
	REQUIRE_FALSE(componentContainer.componentExist<AComponent>(sampleEntity));
}

TEST_CASE("If component exists in the system, and Entity matches, then it exists.",
          "[ComponentContainer][componentExist]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(sampleEntity);
	REQUIRE(componentContainer.componentExist<AComponent>(sampleEntity));
}

TEST_CASE("Any component is invalid when system is empty", "[ComponentContainer][validComponent]") {
	ComponentContainer componentContainer;
	REQUIRE_FALSE(componentContainer.validComponent(1234, (Component*)0xDEADBEEF));
	REQUIRE_FALSE(componentContainer.validComponent<AComponent>(1234, (AComponent*)0xDEADBEEF));
}

TEST_CASE("Any location but real is invalid for given component"
		          "(pointerToReal+offset is invalid for offset other than 0)",
          "[ComponentContainer][validComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	AComponent* sampleComponent = componentContainer.createComponent<AComponent>(sampleEntity);
	REQUIRE_FALSE(componentContainer.validComponent(sampleEntity, sampleComponent + 1));
	REQUIRE_FALSE(componentContainer.validComponent<AComponent>(sampleEntity, sampleComponent - 1));
}

TEST_CASE("Existing component is valid at it's location with right Entity",
          "[ComponentContainer][validComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	AComponent* sampleComponent = componentContainer.createComponent<AComponent>(sampleEntity);
	REQUIRE(componentContainer.validComponent(sampleEntity, (Component*)sampleComponent));
	REQUIRE(componentContainer.validComponent<AComponent>(sampleEntity, sampleComponent));
}

TEST_CASE("getComponent with wrong Entity yields nullptr", "[ComponentContainer][getComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(sampleEntity);
	REQUIRE(componentContainer.getComponent<AComponent>(sampleEntity + 1) == nullptr);
}

TEST_CASE("getComponent with entity which has not component of this type yields nullptr",
          "[ComponentContainer][getComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	REQUIRE(componentContainer.getComponent<AComponent>(sampleEntity) == nullptr);
}

TEST_CASE("getComponent with Entity which had component of this type in the past, but not now yields nullptr",
          "[ComponentContainer][getComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(sampleEntity);
	componentContainer.deleteComponent<AComponent>(sampleEntity);
	REQUIRE(componentContainer.getComponent<AComponent>(sampleEntity) == nullptr);
}

TEST_CASE("getComponent with Entity that have component of desired type yields valid ptr",
          "[ComponentContainer][getComponent]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	AComponent* sampleComponent = componentContainer.createComponent<AComponent>(sampleEntity);
	REQUIRE(componentContainer.getComponent<AComponent>(sampleEntity) == sampleComponent);
}

TEST_CASE("getComponents when there wasn't any components oof desired type yet yields containter with size 0",
          "[ComponentContainer][getComponents]") {
	ComponentContainer componentContainer;
	Components<AComponent> aComponents = componentContainer.getComponents<AComponent>();
	REQUIRE(aComponents.size() == 0);
}

TEST_CASE("getComponents when there are no components now yields contaiener with size 0",
          "[ComponentContainer][getComponents]") {
	ComponentContainer componentContainer;
	Entity sampleEntity = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(sampleEntity);
	componentContainer.deleteComponent<AComponent>(sampleEntity);
	Components<AComponent> components = componentContainer.getComponents<AComponent>();
	REQUIRE(components.size() == 0);
}

TEST_CASE("getComponents when there are components of given type, returns iterable container with these",
          "[ComponentContainer][getComponents]") {
	ComponentContainer componentContainer;
	Entity firstEntity = componentContainer.createEntity();
	Entity secondEntity = componentContainer.createEntity();
	AComponent* firstComponent = componentContainer.createComponent<AComponent>(firstEntity);
	AComponent* secondComponent = componentContainer.createComponent<AComponent>(secondEntity);

	Components<AComponent> aComponents = componentContainer.getComponents<AComponent>();
	REQUIRE(aComponents.size() == 2);

	CAPTURE(firstComponent);
	CAPTURE(secondComponent);

	CAPTURE(&aComponents[0]);
	CAPTURE(&aComponents[1]);

	bool firstIsValid = firstComponent == &aComponents[0] || firstComponent == &aComponents[1];
	REQUIRE(firstIsValid);
	bool secondIsValid = secondComponent == &aComponents[0] || secondComponent == &aComponents[1];
	REQUIRE(secondIsValid);
}

TEST_CASE("Intersection with single component type and empty system puts nothing to container",
          "[ComponentContainer][intersection]") {
	ComponentContainer componentContainer;

	std::vector<AComponent*> aComponents;
	componentContainer.intersection(aComponents);

	REQUIRE(aComponents.empty());
}


TEST_CASE("Intersection with two component types and empty system puts nothing to container",
          "[ComponentContainer][intersection]") {
	ComponentContainer componentContainer;

	std::vector<AComponent*> aComponents;
	std::vector<BComponent*> bComponents;
	componentContainer.intersection(aComponents, bComponents);

	REQUIRE(aComponents.empty());
	REQUIRE(bComponents.empty());
}

TEST_CASE("Intersection with single type, first entity has component other don't, gives containers with size = 1",
          "[ComponentContainer][intersection]") {
	ComponentContainer componentContainer;

	Entity empty = componentContainer.createEntity();

	Entity full = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(full);

	std::vector<AComponent*> components;
	componentContainer.intersection(components);

	REQUIRE(components.size() == 1);
}

TEST_CASE("intersection for each permutation of two components ownership"
		          " gives containers with size 1(only single Entity has both components)",
          "[ComponentContainer][intersection]") {
	ComponentContainer componentContainer;

	Entity entityWithAB = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithAB);
	componentContainer.createComponent<BComponent>(entityWithAB);

	Entity entityWithA = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithA);

	Entity entityWithB = componentContainer.createEntity();
	componentContainer.createComponent<BComponent>(entityWithB);

	std::vector<AComponent*> aComponents;
	std::vector<BComponent*> bComponents;
	componentContainer.intersection(aComponents, bComponents);

	REQUIRE(aComponents.size() == 1);
	REQUIRE(bComponents.size() == 1);
}

TEST_CASE("intersection for each permutation of three components ownership"
		          " gives containers with size 1(only single Entity has all components)",
          "[ComponentContainer][intersection]") {
	ComponentContainer componentContainer;

	Entity entityWithABC = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithABC);
	componentContainer.createComponent<BComponent>(entityWithABC);
	componentContainer.createComponent<CComponent>(entityWithABC);

	Entity entityWithAB = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithAB);
	componentContainer.createComponent<BComponent>(entityWithAB);

	Entity entityWithA = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithA);

	Entity entityWithBC = componentContainer.createEntity();
	componentContainer.createComponent<BComponent>(entityWithBC);
	componentContainer.createComponent<CComponent>(entityWithBC);

	Entity entityWithC = componentContainer.createEntity();
	componentContainer.createComponent<CComponent>(entityWithC);

	Entity entityWithAC = componentContainer.createEntity();
	componentContainer.createComponent<AComponent>(entityWithAC);
	componentContainer.createComponent<CComponent>(entityWithAC);

	std::vector<AComponent*> aComponents;
	std::vector<BComponent*> bComponents;
	std::vector<CComponent*> cComponents;
	componentContainer.intersection(aComponents, bComponents, cComponents);

	REQUIRE(aComponents.size() == 1);
	REQUIRE(bComponents.size() == 1);
	REQUIRE(cComponents.size() == 1);
}


TEST_CASE("createComponent introduces new valid component to system", "[ComponentContainer][createComponent]") {
	ComponentContainer componentContainer;

	Entity owner = componentContainer.createEntity();
	AComponent* componentAdress = componentContainer.createComponent<AComponent>(owner);
	REQUIRE(componentAdress != nullptr);
	CAPTURE(componentAdress);

	REQUIRE(componentContainer.validComponent(owner, componentAdress));
	REQUIRE(componentContainer.componentExist<AComponent>(owner));
	REQUIRE(componentContainer.getComponent<AComponent>(owner) != nullptr);
}

TEST_CASE("deleteComponent removes component from the system", "[ComponentContainer][deleteComponent]") {
	ComponentContainer componentContainer;

	Entity owner = componentContainer.createEntity();
	AComponent* componentAdress = componentContainer.createComponent<AComponent>(owner);

	bool isDelated = componentContainer.deleteComponent<AComponent>(owner);
	REQUIRE(isDelated);

	REQUIRE_FALSE(componentContainer.validComponent(owner, componentAdress));
	REQUIRE_FALSE(componentContainer.componentExist<AComponent>(owner));
	REQUIRE(componentContainer.getComponent<AComponent>(owner) == nullptr);
}

TEST_CASE("deleteEntity removes entity(as defined by entity id and sum of components) from the system",
          "[ComponentContainer][deleteEntity]") {
	ComponentContainer componentContainer;

	Entity someEntity = componentContainer.createEntity();
	AComponent* aComponent = componentContainer.createComponent<AComponent>(someEntity);
	BComponent* bComponent = componentContainer.createComponent<BComponent>(someEntity);

	componentContainer.deleteEntity(someEntity);

	REQUIRE_FALSE(componentContainer.entityExist(someEntity));
	REQUIRE_FALSE(componentContainer.componentExist<AComponent>(someEntity));
	REQUIRE_FALSE(componentContainer.componentExist<BComponent>(someEntity));
}
