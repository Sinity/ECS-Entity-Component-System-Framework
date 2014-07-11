#include "tool/config.h"
#include "componentContainer.h"

class EntityFactory {
public:
	EntityFactory(ComponentContainer& componentContainer) : componentContainer(componentContainer) {}
	bool loadEntities(const std::string& filename, std::string definitionsPath = "");
	Entity createEntity(const std::string& name, ArgsMap addictionalParameters);

private:
	Configuration definitions;
	std::string definitionsPath;

	ComponentContainer& componentContainer;
};
