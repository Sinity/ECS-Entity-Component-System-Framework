#pragma once
#include "ecs/utils/config.h"
#include "componentContainer.h"

/** \brief class for creating whole Entites from definitions outside the code
*
* It allows to define Entity, as collection of components, from text file.
* (initial) Component parameters can be specified in text files,
* which can be also shadowed directly in createEntity call.
*
* Entity definition look like this:
*
* SomeEntityArchetype {
*   SomeComponent {
*       arg1 = 42
*       arg2 = "asdf"
*   }
*
*   PositionComponent {
*       x = 3
*       y = 75
*   }
* }
*
* It consist of entity name, braces enclosing entity definition,
* components names followed by component defnition braces, and inside these braces component parameters,
* in form setting = value.
*/
class EntityFactory {
public:
	EntityFactory(ComponentContainer& componentContainer) :
			componentContainer(componentContainer) {
	}

	/** \brief loads entity definitions form specified file
	*
	* \param filename filename of the entity definitons file.
	* \param definitionsPath path in configuration that points to entity definitions location(root path to definitons)
	*
	* definitionPath in most cases should be empty,
	*   in exception of sharing entity definitions with other configuration.
	*
	* deletes old entites definitions
	*
	* \returns true if Entites are loaded successfully, false otherwise.
	*/
	bool loadEntities(const std::string& filename, std::string definitionsPath = "");

	/** \brief loads entity definitions from in-memory string.
	*
	* \param entitesDefinitions string with entities definitions
	* \param definitionsPath path that points to root where definitions are.
	*
	* deletes old entites definitions
	*
	*/
	void loadEntitesFromMemory(std::string entitiesDefinitions, std::string definitionsPath = "");

	/** \brief creates entity of specified name
	*
	* \param name name of the Entity to create
	* \param addictionalParameters params to be applied on particular components, will shadow definitions parameters
	*
	* addictionalParameters should be in form of list of pairs {"componentName.paramName", "paramValue"}.
	* for example. {{"positionComponent.x", "7"}, {"positionComponent.y", "42}}
	*/
	Entity createEntity(const std::string& name, ArgsMap addictionalParameters);

private:
	Configuration definitions;
	std::string definitionsPath;

	ComponentContainer& componentContainer;
};
