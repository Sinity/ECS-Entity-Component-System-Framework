#pragma once

#include <cassert>
#include <vector>
#include <unordered_map>
#include "tool/config.h"
#include "componentDef.h"

struct ComponentContainerData {
	size_t sizeOfComponent;
	size_t capacity;
	size_t freeIndex;
};

template<typename ComponentClass>
struct Components {
public:
	Components(size_t size, ComponentClass* const components) :
			_size(size),
			components(components) {
	}

	ComponentClass& operator[](size_t index) {
		return components[index];
	}

	size_t size() {
		return _size;
	}

private:
	size_t _size;
	ComponentClass* const components;
};

class ComponentContainer {
public:
	ComponentContainer(Configuration& config) :
			logger("ComponentContainer"),
			config(config) {

		createNullEntity();
	}

	~ComponentContainer() {
		for(auto& container : containers) {
			freeContainer(container);
		}
	}

	bool componentExist(ComponentHandle component) {
		return componentHandles.find(component) != componentHandles.end();
	}

	template<typename ComponentClass>
	ComponentClass* getComponent(ComponentHandle component) {
		auto it = componentHandles.find(component);
		return it != componentHandles.end() ? (ComponentClass*)it->second : nullptr;
	}

	template<typename ComponentClass>
	ComponentClass* getComponent(Entity owner) {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		if(containerIndex >= containers.size()) {
			initializeContainersTo(containerIndex);
			return nullptr;
		}
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		return (ComponentClass*)findComponent(owner, container);
	}

	template<typename ComponentClass>
	Components<ComponentClass> getComponents() {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		if(containerIndex > containers.size()) {
			initializeContainersTo(containerIndex);
		}

		return {containers[containerIndex].second.freeIndex, (ComponentClass*)containers[containerIndex].first};
	}

	template<typename HeadComponentType, typename... TailComponents>
	void intersection(std::vector<HeadComponentType*>& head, std::vector<TailComponents*>& ... tail) {
		size_t containerIndex = ContainerID::value<HeadComponentType>();
		if(containerIndex >= containers.size()) {
			initializeContainersTo(containerIndex);
			return;
		}
		auto& headContainer = containers[containerIndex];
		if(headContainer.first == nullptr) {
			return;
		}

		HeadComponentType* headComponents = (HeadComponentType*)headContainer.first;
		for(size_t i = 0; i < headContainer.second.freeIndex; i++) {
			if(allComponentsExist(headComponents[i].owner, tail...)) {
				head.emplace_back(&headComponents[i]);
			}
		}
	}

	template<typename ComponentClass>
	ComponentClass& createComponent(Entity owner, ArgsMap args = ArgsMap()) {
		assert(entityExist(owner));

		auto container = prepareComponentContainer<ComponentClass>();
		if(!container) {
			logger.fatal("Cannot prepare component container. Application will be terminated. Probably we don't have any memory.");
			exit(1);
		}

		char* adress = preparePlaceForNewComponent<ComponentClass>(owner);
		ComponentClass* createdComponent = new(adress) ComponentClass(owner, nextComponentHandle);
		container->second.freeIndex++;

		componentHandles[nextComponentHandle] = createdComponent;
		nextComponentHandle++;

		if(!args.empty()) {
			createdComponent->init(args);
		}
		return *createdComponent;
	}

	template<typename ComponentClass>
	void deleteComponent(Entity owner) {
		assert(entityExist(owner));

		//locate container
		size_t containerIndex = ContainerID::value<ComponentClass>();
		if(containerIndex >= containers.size()) {
			logger.warn("Cannot delete component with owner ", (unsigned int)owner, ": container don't exist");
			initializeContainersTo(containerIndex);
			return;
		}
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			logger.warn("Cannot delete component with owner ", (unsigned int)owner, ": container don't exist");
			return;
		}

		//locate component
		ComponentClass* component = (ComponentClass*)findComponent(owner, container);
		if(!component) {
			logger.warn("Cannot delete component with owner ", (unsigned int)owner, ": component not in container");
			return;
		}

		componentHandles.erase(component->handle);
		component->~ComponentClass();

		fillHoleAfterComponent(container, (char*)component);
		container.second.freeIndex--;

		size_t index = component - (ComponentClass*)container.first;
		refreshComponentHandles(container, index);
	}

	bool entityExist(Entity entityID) {
		return entityID < entityExistingTable.size() && entityExistingTable[entityID];
	}

	Entity createEntity() {
		entityExistingTable.push_back(true);
		return Entity(entityExistingTable.size() - 1);
	}

	void deleteEntity(Entity owner) {
		assert(entityExist(owner));

		for(auto& container : containers) {
			Component* currentComponent = findComponent(owner, container);
			if(currentComponent) {
				componentHandles.erase(currentComponent->handle);
				currentComponent->~Component();

				fillHoleAfterComponent(container, (char*)currentComponent);
				container.second.freeIndex--;

				size_t index = ((char*)currentComponent - container.first) / container.second.sizeOfComponent;
				refreshComponentHandles(container, index);
			}
		}
		entityExistingTable[owner] = false;
	}

public:
	Logger logger;
private:
	std::vector<char> entityExistingTable;

	std::vector<std::pair<char*, ComponentContainerData>> containers;
	std::unordered_map<ComponentHandle, Component*> componentHandles;
	ComponentHandle nextComponentHandle = 1;

	Configuration& config;


	template<typename ComponentClass>
	std::pair<char*, ComponentContainerData>* prepareComponentContainer() {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		if(containerIndex >= containers.size()) {
			initializeContainersTo(containerIndex);
			return allocateNewContainer<ComponentClass>() ? &containers[containerIndex] : nullptr;
		}
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return allocateNewContainer<ComponentClass>() ? &container : nullptr;
		}

		bool containerIsFull = container.second.freeIndex == container.second.capacity;
		if(containerIsFull) {
			char* oldContainerAdress = container.first;
			if(!increaseContainerCapacity(container)) {
				return nullptr;
			}
			if(oldContainerAdress != container.first) {
				refreshComponentHandles(container);
			}
		}

		return &container;
	}

	void refreshComponentHandles(std::pair<char*, ComponentContainerData>& container, size_t startIndex = 0) {
		for(size_t i = startIndex; i < container.second.freeIndex; i++) {
			Component& elem = *(Component*)(container.first + i * container.second.sizeOfComponent);
			componentHandles[elem.handle] = &elem;
		}
	}

	bool increaseContainerCapacity(std::pair<char*, ComponentContainerData>& container) {
		size_t newCapacity = container.second.capacity * config.get("componentContainer.growFactor", 16);
		char* newContainerAdress = (char*)realloc(container.first, newCapacity * container.second.sizeOfComponent);
		if(!newContainerAdress) {
			newCapacity = container.second.capacity + 1;
			newContainerAdress = (char*)realloc(container.first, newCapacity * container.second.sizeOfComponent);
			if(!newContainerAdress) {
				logger.fatal("Cannot resize component container, even by 1 element. Desired capacity: ", newCapacity, ", sizeof(Type): ", container.second.sizeOfComponent);
				assert(!"resizeContainer: cannot allocate memory for new element");
				return false;
			}
		}

		container.second.capacity = newCapacity;
		container.first = newContainerAdress;
		return true;
	}

	template<typename ComponentClass>
	bool allocateNewContainer() {
		ComponentContainerData metadata;
		metadata.capacity = config.get("componentContainer.initialCapacity", 4096);
		metadata.sizeOfComponent = sizeof(ComponentClass);
		metadata.freeIndex = 0;

		char* newContainer = (char*)malloc(sizeof(ComponentClass) * metadata.capacity);
		if(!newContainer) {
			metadata.capacity = 1;
			newContainer = (char*)malloc(sizeof(ComponentClass));
			if(!newContainer) {
				logger.fatal("Cannot create new container, even for 1 element. sizeof(Type): ", sizeof(ComponentClass));
				assert(!"Create component: cannot allocate memory for new container; even for 1 element");
				return false;
			}
		}

		containers[ContainerID::value<ComponentClass>()] = {newContainer, metadata};
		return true;
	}

	void freeContainer(std::pair<char*, ComponentContainerData>& container) {
		if(container.first != nullptr) {
			for(size_t i = 0; i < container.second.freeIndex; i++) {
				Component* currentComponent = (Component*)(container.first + i * container.second.sizeOfComponent);
				currentComponent->~Component();
			}

			free(container.first);
		}
	}

	Component* findComponent(Entity owner, std::pair<char*, ComponentContainerData>& container) {
		char* const components = container.first;
		int min = 0;
		int max = container.second.freeIndex - 1;

		while(max >= min) {
			size_t mid = min + (max - min) / 2;
			Component* currentComponent = (Component*)(components + mid * container.second.sizeOfComponent);

			if(currentComponent->owner == owner) {
				return currentComponent;
			}
			else if(currentComponent->owner < owner) {
				min = mid + 1;
			}
			else {
				max = mid - 1;
			}
		}

		return nullptr;
	}

	template<typename ComponentClass>
	char* preparePlaceForNewComponent(Entity owner) {
		auto& container = containers[ContainerID::value<ComponentClass>()];

		if(container.second.freeIndex == 0) {
			return container.first;
		}

		ComponentClass* endOfContainer = (ComponentClass*)container.first + container.second.freeIndex;
		ComponentClass* lastComponent = endOfContainer - 1;
		if(lastComponent->owner > owner) {
			ComponentClass* place = findPlaceForNewComponent<ComponentClass>(owner);
			memmove(place + 1, place, (endOfContainer - place) * container.second.sizeOfComponent);

			size_t index = place + 1 - (ComponentClass*)container.first;
			refreshComponentHandles(container, index);
			componentHandles[endOfContainer->handle] = endOfContainer;

			return (char*)place;
		}

		return (char*)endOfContainer;
	}


	template<typename ComponentClass>
	ComponentClass* findPlaceForNewComponent(Entity owner) {        //TODO: sth. like binary search
		auto& container = containers[ContainerID::value<ComponentClass>()];
		ComponentClass* const components = (ComponentClass*)container.first;

		for(size_t i = 0; i < container.second.freeIndex; i++) {
			if(components[i].owner > owner) {
				return &components[i];
			}
		}
		return &components[container.second.freeIndex];
	}


	void fillHoleAfterComponent(std::pair<char*, ComponentContainerData>& container, char* removedComponent) {
		char* endOfContainer = container.first + container.second.freeIndex * container.second.sizeOfComponent;
		if(removedComponent != (endOfContainer - container.second.sizeOfComponent)) {
			size_t bytesToMove = endOfContainer - removedComponent - container.second.sizeOfComponent;
			memmove(removedComponent, removedComponent + container.second.sizeOfComponent, bytesToMove);
		}
	}

	void initializeContainersTo(size_t index) {
		ComponentContainerData null;
		null.capacity = 0;
		null.sizeOfComponent = 0;
		null.freeIndex = 0;

		for(size_t i = 0; i < index - containers.size() + 1; i++) {
			containers.emplace_back(nullptr, null);
		}
	}

	template<typename HeadComponentType, typename... TailComponents>
	bool allComponentsExist(Entity entity, std::vector<HeadComponentType*>& head, std::vector<TailComponents*>& ... tail) {
		size_t containerIndex = ContainerID::value<HeadComponentType>();
		if(containerIndex >= containers.size()) {
			initializeContainersTo(containerIndex);
			return nullptr;
		}
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		HeadComponentType* component = (HeadComponentType*)findComponent(entity, container);
		if(!component) {
			return false;
		}

		if(allComponentsExist(tail...)) {
			head.emplace_back(component);
			return true;
		} else {
			return false;
		}
	}

	template<typename LastComponentType>
	bool allComponentsExist(Entity entity, std::vector<LastComponentType*>& last) {
		size_t containerIndex = ContainerID::value<LastComponentType>();
		if(containerIndex >= containers.size()) {
			initializeContainersTo(containerIndex);
			return nullptr;
		}
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		LastComponentType* component = (LastComponentType*)findComponent(entity, container);
		if(!component) {
			return false;
		}

		last.emplace_back(component);
		return true;
	}

	void createNullEntity() {
		entityExistingTable.emplace_back(false);
	}

	class ContainerID {
	public:
		template<typename T>
		static size_t value() {
			static size_t id = counter++;
			return id;
		}

	private:
		static size_t counter;
	};
};
