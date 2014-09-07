#include "entityFactory.h"
#include "componentFactory.h"

bool EntityFactory::loadEntities(const std::string& filename, std::string definitionsPath) {
	this->definitionsPath = std::move(definitionsPath);
	return definitions.load(filename);
}

Entity EntityFactory::createEntity(const std::string& name, ArgsMap addictionalParameters) {
	ConfigNode* entityDefinitions = definitions.getNode(definitionsPath);
	auto desiredEntity = entityDefinitions->childs.find(name);
	if(desiredEntity == entityDefinitions->childs.end()) {
		return 0;
	}

	Entity entity = componentContainer.createEntity();
	for(auto& component : desiredEntity->second->childs) {
		auto componentSettings = component.second->settings;

		for(auto& param : addictionalParameters) {
			std::vector<std::string> splittedParameterPath = split(param.first, '.');
			if(splittedParameterPath.size() != 2) { //format must be component.setting = something
				componentContainer.deleteEntity(entity);
				return 0;
			}

			if(splittedParameterPath[0] == component.first) {
				componentSettings[splittedParameterPath[1]] = param.second;
			}
		}

		Component* currComponent = ComponentFactory::createComponent(componentContainer, component.first,
		                                                             entity, componentSettings);
		if(!currComponent) {
			componentContainer.deleteEntity(entity);
			return 0;
		}
	}

	return entity;
}
