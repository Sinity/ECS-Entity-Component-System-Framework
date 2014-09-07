#pragma once
#include <cassert>
#include <vector>
#include <cstring>
#include <unordered_map>
#include "tool/logger.h"
#include "componentDef.h"

template<typename ComponentClass>
struct Components;

class ComponentContainer {
public:
	ComponentContainer() :
			logger("ComponentContainer") {
		createNullEntity();
		configure();
	}

	void configure(unsigned int maxComponentTypes = 4096, unsigned int growFactor = 16,
	               unsigned int initialCapacity = 4096) {
		initializeContainersTo(maxComponentTypes); //If amount of component types > this, access violation will happen
		this->growFactor = growFactor > 2 ? growFactor : 2;
		this->initialCapacity = initialCapacity;
	}

	~ComponentContainer() {
		for(auto& container : containers) {
			freeContainer(container);
		}
	}

	template<typename ComponentClass>
	bool componentExist(Entity owner) {
		return getComponent<ComponentClass>(owner) != nullptr;
	}

	bool componentExist(Entity owner, Component* adress) {
		for(unsigned int i = 0; i < containers.size(); i++) {
			//it's because we must be sure that it's safe to access this memory, and there always can be collision...
			//if owner is for. ex 1 then probability of collision with other untangled data is high
			if(containers[i].first <= (char*)adress && (char*)adress <= (containers[i].first +
					containers[i].second.sizeOfComponent * containers[i].second.freeIndex)) {
				if(adress->owner == owner) {
					return true;
				} else {
					return false;
				}
			}
		}
		return false;
	}

	template<typename ComponentClass>
	ComponentClass* getComponent(Entity owner) {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		return (ComponentClass*)findComponent(owner, container);
	}

	template<typename ComponentClass>
	Components<ComponentClass> getComponents() {
		size_t containerIndex = ContainerID::value<ComponentClass>();

		size_t size = containers[containerIndex].second.freeIndex;
		ComponentClass* components = (ComponentClass*)containers[containerIndex].first;
		bool isValidContainer = containers[containerIndex].first != nullptr;
		return {size, components, isValidContainer};
	}

	template<typename HeadComponentType, typename... TailComponents>
	void intersection(std::vector<HeadComponentType*>& head, std::vector<TailComponents*>& ... tail) {
		Components<HeadComponentType> headComponents = getComponents<HeadComponentType>();
		if(!headComponents.valid) {
			return;
		}

		for(size_t i = 0; i < headComponents.size(); i++) {
			if(allComponentsExist(headComponents[i].owner, tail...)) {
				head.emplace_back(&headComponents[i]);
			} else {
				wipeLastComponentIfExceedsSize(head.size(), tail...);
			}
		}
	}

	template<typename ComponentClass>
	ComponentClass* createComponent(Entity owner, ArgsMap args = ArgsMap()) {
		auto container = prepareComponentContainer<ComponentClass>();
		if(!container) {
			logger.fatal("Cannot prepare component container. Probably we don't have any memory left.");
			return nullptr;
		}

		char* adress = preparePlaceForNewComponent<ComponentClass>(owner);
		ComponentClass* createdComponent = new(adress) ComponentClass(owner);
		container->second.freeIndex++;

		if(!args.empty()) {
			createdComponent->init(args);
		}

		return createdComponent;
	}

	template<typename ComponentClass>
	bool deleteComponent(Entity owner) {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			logger.warn("Cannot delete component with owner ", owner, ": container don't exist");
			return false;
		}

		ComponentClass* componentToDelete = (ComponentClass*)findComponent(owner, container);
		if(!componentToDelete) {
			logger.warn("Cannot delete component with owner ", owner, ": component not in container");
			return false;
		}

		componentToDelete->~ComponentClass();
		fillHoleAfterComponent(container, (char*)componentToDelete);
		container.second.freeIndex--;

		return true;
	}

	bool deleteComponent(Entity owner, Component* componentToDelete) {
		for(unsigned int i = 0; i < containers.size(); i++) {
			if(containers[i].first <= (char*)componentToDelete && (char*)componentToDelete <= (containers[i].first +
					containers[i].second.sizeOfComponent * containers[i].second.freeIndex)) {
				if(componentToDelete->owner != owner) {
					logger.warn("Cannot delete component by adress: owner's mismatch, probably alredy deleted",
					            "or data is corrupted. Desired entity: ", owner, ", Real entity: ",
					            componentToDelete->owner, ".");

					return false;
				}

				componentToDelete->~Component();
				fillHoleAfterComponent(containers[i], (char*)componentToDelete);
				containers[i].second.freeIndex--;
				return true;
			}
		}

		logger.warn("Cannot delete component. Given adress doesn't seem valid. It's outside all containers");
		return false;
	}

	bool entityExist(Entity entityID) {
		return entityID < entityExistingTable.size() && entityExistingTable[entityID];
	}

	Entity createEntity() {
		entityExistingTable.push_back(true);
		return entityExistingTable.size() - 1;
	}

	void deleteEntity(Entity owner) {
		for(auto& container : containers) {
			if(container.first == nullptr) {
				continue;
			}
			Component* currentComponent = findComponent(owner, container);
			if(currentComponent) {
				currentComponent->~Component();
				fillHoleAfterComponent(container, (char*)currentComponent);
				container.second.freeIndex--;
			}
		}
		entityExistingTable[owner] = false;
	}

public:
	Logger logger;

private:
	std::vector<char> entityExistingTable;

	struct ComponentContainerData {
		size_t sizeOfComponent;
		size_t capacity;
		size_t freeIndex;
	};
	std::vector<std::pair<char*, ComponentContainerData>> containers;
	unsigned int initialCapacity;
	unsigned int growFactor;

	template<typename ComponentClass>
	std::pair<char*, ComponentContainerData>* prepareComponentContainer() {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return allocateNewContainer<ComponentClass>() ? &container : nullptr;
		}

		bool containerIsFull = container.second.freeIndex == container.second.capacity;
		if(containerIsFull) {
			if(!increaseContainerCapacity(container)) {
				return nullptr;
			}
		}

		return &container;
	}

	bool increaseContainerCapacity(std::pair<char*, ComponentContainerData>& container) {
		size_t newCapacity = container.second.capacity * growFactor;
		char* newContainerAdress = (char*)realloc(container.first, newCapacity * container.second.sizeOfComponent);
		if(!newContainerAdress) {
			newCapacity = container.second.capacity + 1;
			newContainerAdress = (char*)realloc(container.first, newCapacity * container.second.sizeOfComponent);
			if(!newContainerAdress) {
				logger.fatal("Cannot resize component container, even by 1 element. Desired capacity: ", newCapacity,
				             ", sizeof(Type): ", container.second.sizeOfComponent);
				return false;
			}
		}

		container.second.capacity = newCapacity;
		container.first = newContainerAdress;
		return true;
	}

	template<typename ComponentClass>
	bool allocateNewContainer() {
		ComponentContainerData containerMetadata;
		containerMetadata.capacity = initialCapacity;
		containerMetadata.sizeOfComponent = sizeof(ComponentClass);
		containerMetadata.freeIndex = 0;

		char* containerMemory = (char*)malloc(sizeof(ComponentClass) * containerMetadata.capacity);
		if(!containerMemory) {
			containerMetadata.capacity = 1;
			containerMemory = (char*)malloc(sizeof(ComponentClass));
			if(!containerMemory) {
				logger.fatal("Cannot create new container, even for 1 element. sizeof(Type): ", sizeof(ComponentClass));
				return false;
			}
		}

		containers[ContainerID::value<ComponentClass>()] = {containerMemory, containerMetadata};
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

			if(currentComponent->owner < owner) {
				min = mid + 1;
			} else if(currentComponent->owner > owner) {
				max = mid - 1;
			} else {
				return currentComponent;
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
			return (char*)place;
		}

		return (char*)endOfContainer;
	}


	template<typename ComponentClass>
	ComponentClass* findPlaceForNewComponent(Entity owner) {
		auto& container = containers[ContainerID::value<ComponentClass>()];
		ComponentClass* const components = (ComponentClass*)container.first;

		int min = 0;
		int max = container.second.freeIndex - 1;

		while(max >= min) {
			size_t mid = min + (max - min) / 2;
			ComponentClass* currentComponent = components + mid;

			if(currentComponent->owner > owner) {
				if((currentComponent - 1)->owner < owner) {
					return currentComponent;
				}
				max = mid - 1;
			}
			else {
				min = mid + 1;
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

	void initializeContainersTo(size_t size) {
		if(size < containers.size()) {
			return;
		}

		ComponentContainerData null;
		null.capacity = 0;
		null.sizeOfComponent = 0;
		null.freeIndex = 0;

		containers.reserve(size);
		for(size_t i = 0; i < size - containers.size(); i++) {
			containers.emplace_back(nullptr, null);
		}
	}

	template<typename HeadComponentType, typename... TailComponents>
	bool allComponentsExist(Entity entity, std::vector<HeadComponentType*>& head,
	                        std::vector<TailComponents*>& ... tail) {
		size_t containerIndex = ContainerID::value<HeadComponentType>();
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		HeadComponentType* component = (HeadComponentType*)findComponent(entity, container);
		if(!component) {
			return false;
		}

		if(allComponentsExist(entity, tail...)) {
			head.emplace_back(component);
			return true;
		} else {
			return false;
		}
	}

	template<typename LastComponentType>
	bool allComponentsExist(Entity entity, std::vector<LastComponentType*>& last) {
		size_t containerIndex = ContainerID::value<LastComponentType>();
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

	template<typename HeadComponentType, typename... TailComponents>
	void wipeLastComponentIfExceedsSize(size_t size, std::vector<HeadComponentType*>& head,
	                                    std::vector<TailComponents*>& ... tail) {
		if(head.size() > size) {
			head.pop_back();
		}
		wipeLastComponentIfExceedsSize(size, tail...);
	}

	template<typename LastComponentType>
	void wipeLastComponentIfExceedsSize(size_t size, std::vector<LastComponentType*>& last) {
		if(last.size() > size) {
			last.pop_back();
		}
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

template<typename ComponentClass>
struct Components {
public:
	Components(size_t size, ComponentClass* const components, bool valid) :
			valid(valid),
			_size(size),
			components(components) {
	}

	ComponentClass& operator[](size_t index) {
		return components[index];
	}

	size_t size() {
		return _size;
	}

	bool valid = false;
private:
	size_t _size;
	ComponentClass* const components;
};
