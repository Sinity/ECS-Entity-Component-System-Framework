#include "entityFactory.h"
#include "componentFactory.h"

bool EntityFactory::loadEntities(const std::string& filename, std::string definitionsPath) {
	this->definitionsPath = std::move(definitionsPath);
	return definitions.load(filename);
}

Entity EntityFactory::createEntity(const std::string& name, ArgsMap addictionalParameters) {
	ConfigNode* entities = definitions.getNode(definitionsPath);
	auto desiredEntity = entities->childs.find(name);
	if(desiredEntity == entities->childs.end()) {
		return Entity(0);
	}

	Entity entity = componentContainer.createEntity();
	for(auto& component : desiredEntity->second->childs) {
		auto componentSettings = component.second->settings;

		for(auto& param : addictionalParameters) {
			std::vector<std::string> splittedParameterPath = split(param.first, '.');
			assert(splittedParameterPath.size() == 2); //format component.setting = something.
			if(splittedParameterPath[0] == component.first) {
				componentSettings[splittedParameterPath[1]] = param.second;
			}
		}

		Component* currComponent = ComponentFactory::createComponent(componentContainer, component.first,
		                                                             entity, componentSettings);
		assert(currComponent && "Wrong component name in entity definition or component is not registered!");
		//TODO: Better error handling. Must create method for deleting component without knowing exact type of it.
	}
	return entity;
}
