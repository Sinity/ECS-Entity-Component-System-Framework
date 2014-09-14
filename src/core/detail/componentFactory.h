#pragma once
#include <unordered_map>
#include <memory>
#include "core/componentContainer.h"

class ComponentFactory {
private:
	class BaseComponentCreator {
	public:
		virtual Component* create(ComponentContainer& container, Entity entity, ArgsMap args) = 0;
	};

	template<typename Component>
	class ComponentCreator : public BaseComponentCreator {
		Component* create(ComponentContainer& container, Entity entity, ArgsMap args) {
			Component* createdComponent = container.createComponent<Component>(entity, std::move(args));
			return createdComponent;
		}
	};

public:
	template<typename Component>
	static void registerComponent(std::string componentName) {
		creators()[componentName].reset(new ComponentCreator<Component>);
	}

	static Component* createComponent(ComponentContainer& componentContainer, const std::string& componentName,
	                                  Entity entity, ArgsMap args) {
		auto it = creators().find(componentName);
		if(it == creators().end()) {
			return nullptr;
		}
		return (*it).second->create(componentContainer, entity, std::move(args));
	}

private:
	using CreatorsMap = std::unordered_map<std::string, std::unique_ptr<BaseComponentCreator>>;

	static CreatorsMap& creators() {
		static CreatorsMap* creators = new CreatorsMap;
		return *creators;
	}
};

template<typename ComponentClass>
class ComponentFactoryRegistrator {
public:
	ComponentFactoryRegistrator(std::string name) {
		ComponentFactory::registerComponent<ComponentClass>(name);
	}
};
