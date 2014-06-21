#define BOOST_TEST_MODULE Components
#include <boost/test/unit_test.hpp>
#include "core/componentContainer.h"
#include "tool/logger.h"
#include "tool/config.h"

struct SampleComponent : public Component {
	static const ComponentType type = ComponentType::SampleComponent;


	SampleComponent(Entity owner, ComponentHandle handle) :
	  	Component(owner, handle) {
	}


	int content = 0;
};

struct AnotherSampleComponent : public Component {
    static const ComponentType type = ComponentType::AnotherSampleComponent;

    AnotherSampleComponent(Entity owner, ComponentHandle handle) :
        Component(owner, handle) {
    }

    bool state = false;
};


BOOST_AUTO_TEST_CASE(component_container_create_entity_test) {
    Logger logger;
    Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity test1 = components.createEntity();
	Entity test2 = components.createEntity();

	BOOST_CHECK_NE(test1, test2);
}


BOOST_AUTO_TEST_CASE(component_container_entity_exist_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity e1 = components.createEntity();
	Entity e2 = components.createEntity();

	BOOST_ASSERT(components.entityExist(e1) && components.entityExist(e2));

	components.deleteEntity(e1);
	BOOST_ASSERT(!components.entityExist(e1));
	BOOST_ASSERT(components.entityExist(e2));
}


BOOST_AUTO_TEST_CASE(component_container_delete_entity_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity entity = components.createEntity();
	Entity entity2 = components.createEntity();


	ComponentHandle component = components.createComponent<SampleComponent>(entity).handle;

	components.deleteEntity(entity2);
	BOOST_ASSERT(components.componentExist(component));
	components.deleteEntity(entity);
	BOOST_ASSERT(!components.componentExist(component));

	BOOST_ASSERT(!components.entityExist(entity));
	BOOST_ASSERT(!components.entityExist(entity2));
}


BOOST_AUTO_TEST_CASE(component_container_component_exist_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity entity = components.createEntity();

	ComponentHandle component = components.createComponent<SampleComponent>(entity).handle;
	BOOST_ASSERT(components.componentExist(component));

	components.deleteComponent<SampleComponent>(entity);
	BOOST_ASSERT(!components.componentExist(component));
}


BOOST_AUTO_TEST_CASE(component_container_getting_components_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity entity = components.createEntity();
	Entity entity2 = components.createEntity();

	ComponentHandle component = components.createComponent<SampleComponent>(entity).handle;
	components.createComponent<SampleComponent>(entity2);

	BOOST_ASSERT(components.getComponent<SampleComponent>(component));
	BOOST_ASSERT(components.getComponent<SampleComponent>(entity) == components.getComponent<SampleComponent>(component));

	Components<SampleComponent> sampleComponents = components.getComponents<SampleComponent>();

	BOOST_CHECK_EQUAL(sampleComponents.size(), 2);
	BOOST_CHECK_EQUAL(sampleComponents[0].handle, component);
	BOOST_CHECK_EQUAL(sampleComponents[1].owner, entity2);
}

//BOOST_AUTO_TEST_CASE(component_container_intersection_test) { //TODO: fix interpolation() on windows, somehow, and then uncomment it
//NullLogger logger;
//Configuration config(logger);
//ComponentContainer components(logger, config);
//
//    Entity entity = components.createEntity();
//    components.createComponent<SampleComponent>(entity);
//    components.createComponent<AnotherSampleComponent>(entity);
//
//    components.createComponent<SampleComponent>(components.createEntity());
//    components.createComponent<AnotherSampleComponent>(components.createEntity());
//
//    
//    Entity entity2 = components.createEntity();
//    components.createComponent<SampleComponent>(entity2);
//    components.createComponent<AnotherSampleComponent>(entity2);
//
//    std::vector<SampleComponent*> sampleComponents;
//    std::vector<AnotherSampleComponent*> anotherSampleComponents;
//    components.intersection(sampleComponents, anotherSampleComponents);
//
//    BOOST_CHECK_EQUAL(sampleComponents.size(), 2);
//    BOOST_CHECK_EQUAL(sampleComponents[0]->owner, anotherSampleComponents[0]->owner);
//    BOOST_CHECK_EQUAL(sampleComponents[0]->owner, entity);
//}

BOOST_AUTO_TEST_CASE(component_container_creating_components_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity entity = components.createEntity();
	Entity entity2 = components.createEntity();

	SampleComponent& c1 = components.createComponent<SampleComponent>(entity2);
	c1.content = 111;

	SampleComponent& c2 = components.createComponent<SampleComponent>(entity);
	c2.content = 222;

	SampleComponent& new_c1 = *components.getComponent<SampleComponent>(entity2);
	SampleComponent& new_c2 = *components.getComponent<SampleComponent>(entity);


	BOOST_CHECK_EQUAL(new_c1.content, 111);  //check if content is not broken

	//these tests can be invalid if implementation changes(very unlikely)
	BOOST_ASSERT(&c1 != &new_c1); //adress should change because of older entity created component, thus all newer components should be shifted to make place.
	BOOST_ASSERT(&new_c1 > &new_c2); //Components should be sorted by owner
    BOOST_ASSERT(&new_c1 == &new_c2+1); //in continious memory
}


BOOST_AUTO_TEST_CASE(component_container_delete_component_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	Entity entity = components.createEntity();
	Entity entity2 = components.createEntity();

    SampleComponent& c1 = components.createComponent<SampleComponent>(entity);
    SampleComponent& c2 = components.createComponent<SampleComponent>(entity2);
    c2.content = 666;

    components.deleteComponent<SampleComponent>(c1.owner);
    BOOST_ASSERT(components.getComponent<SampleComponent>(entity) == nullptr);

    SampleComponent& new_c2 = *components.getComponent<SampleComponent>(entity2);
    BOOST_CHECK_EQUAL(new_c2.content, 666);
}



BOOST_AUTO_TEST_CASE(component_container_general_and_stress_test) {
	Logger logger;
	Configuration config(logger);
	ComponentContainer components(logger, config);

	//create dummy component to be sure if we aren't just lucky(first component may work but rest no)
	components.createComponent<SampleComponent>(components.createEntity());

	//create entity
	Entity sampleEntity = components.createEntity();
	BOOST_CHECK(components.entityExist(sampleEntity));

	//create first component
	ComponentHandle component = components.createComponent<SampleComponent>(sampleEntity).handle;
	BOOST_CHECK(components.componentExist(component));

	//find this component using it's owner
	ComponentHandle theSame = components.getComponent<SampleComponent>(sampleEntity)->handle;
	BOOST_CHECK_EQUAL(theSame, component);

	//get this component directly using ComponentHandle
	SampleComponent* componentPtr = components.getComponent<SampleComponent>(component);
	componentPtr->content = 666;

	//create some components, delete some components, delete some entities...
	for(unsigned int i = 0; i < 10000; i++) {
		Entity newEntity = components.createEntity();
		components.createComponent<SampleComponent>(newEntity);
		if(i % 100 == 0) {
			components.deleteComponent<SampleComponent>(newEntity);
		}
		if(i % 666 == 0)
			components.deleteEntity(newEntity);
	}

	//check if system still works, can find original component
	ComponentHandle stillTheSame = components.getComponent<SampleComponent>(sampleEntity)->handle;
	BOOST_CHECK_EQUAL(stillTheSame, component);

	//check mapping pointers to ComponentHandles. It can, and probably will have different adress compared to componentReference above. (thats why we need ComponentHandles)
	SampleComponent* ptrToTheSameComponent = components.getComponent<SampleComponent>(stillTheSame);
	BOOST_CHECK_EQUAL(ptrToTheSameComponent->content, 666);
}
