#include "entityFactory.h"
#include "componentFactory.h"

bool EntityFactory::loadEntities(const std::string& filename, std::string definitionsPath) {
	definitionsPath = std::move(definitionsPath);
	return definitions.load(filename);
}

Entity EntityFactory::createEntity(const std::string& name, ArgsMap addictionalParameters) {
	ConfigNode* entities = definitions.getNode(definitionsPath);
	auto desiredEntity = entities->childs.find(name);
	if(desiredEntity == entities->childs.end()) {
		return Entity(0);
	}

	Entity entity = componentContainer.createEntity();
	for(auto& component : (*desiredEntity).second->childs) {
		auto componentSettings = component.second->settings;

		for(auto& param : addictionalParameters) {
			std::vector<std::string> splittedParameterPath = split(param.first, '.');
			assert(splittedParameterPath.size() == 2);
			if(splittedParameterPath[0] == component.first) {
				componentSettings[splittedParameterPath[1]] = param.second;
			}
		}

		ComponentFactory::createComponent(componentContainer, component.first, entity, componentSettings);
	}
	return entity;
}
