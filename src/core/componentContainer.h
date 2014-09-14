#pragma once
#include <cassert>
#include <vector>
#include <cstring>
#include <unordered_map>
#include "ecs/utils/logger.h"
#include "componentDef.h"

template<typename ComponentClass>
struct Components;

/** \brief container for components from all types in the system
*
* It's the core of this Entity-Component-System implementation. It stores all components and gives means to
* operate on them.
*
* Each component has special id of type Entity, called owner, that describes to what 'Entity' it logically belongs.
* All components with the same owner belongs to single Entity. Owner id's are assigned by this class.
*
* To create component, use createComponent method. You must have valid Entity first.
* ComponentContainer componentContainer;
* Entity someEntity = componentContainer.createEntity();
* PositionComponent* positionComponent = componentContainer.createComponent<PositionComponent>(someEntity);
*
* You can get all components of some type O(1), by calling getComponents.
* Components of given type are arranged continiously in the memory, so this is very efficent(cache) for
* processing components type by type, what should be done in most cases when using ECS.
*
* You can also get component of arbitrary type from some Entity, in O(lg n), by calling getComponent.
*
* Another useful method for access of components is intersection, which can be thinked as method returning list
* of Entities that have at least all specified components. It fills passed containers with components of these types,
* holding useful property that the same index specify the same Entity in all of containers, and thus all containers
* have the same size.
*
* You shouldn't keep any references to components for a long time. They can invalidate when creating or deleting
* components. In case you aren't sure if reference is valid, call validComponent method.
*
* You could delete component by calling deleteComponent. This can be slow - it needs to shift any components with
* owners that are more recent than component we are trying to delete to the left, to fill the gape. Theoretically,
* it's O(n). createComponent has the same pessimistic complexity, but it is more likely to create components belonging
* to recent entities instead of old, so in many cases it will be O(1).
*
* You can delete Entity with deleteEntity. It will delete all components belonging to it, amd Entity itself.
*/
class ComponentContainer {
public:
	ComponentContainer() :
			logger("ComponentContainer") {
		createNullEntity();
		configure();
	}

	/** \brief configure various class settings
	*
	* \param maxComponentTypes define number of supported Component types. If at any time actual amount of types exceed
	*   this, errors and memory access/write violations will happen.
	*
	* \param growFactor defines how fast will containers grow. When container runs out of space, it will extend to
	*   growFactor * currentSize. Small - minimum memory usage, often reallocations,
	*   big - possibly wasted space, rare reallocations
	*
	* \param initialCapacity initial capacity of all particular component type containers
	*/
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

	/** \brief tests if component exists in the system
	*
	* \param owner Entity that desired component belogs to
	*
	* ComponentClass template parameter is type of component that we want to check.
	* Computational Complexity of this routine is O(log(n)), where n is amount of components of the same type
	* Uses iterative binary search
	*/
	template<typename ComponentClass>
	bool componentExist(Entity owner) {
		return getComponent<ComponentClass>(owner) != nullptr;
	}

	/** \brief tests validity of component handle(pointer) if it points to location inside component container
	*       and component at this location belongs to desired owner
	*
	*   \param owner Entity that desired component belongs to
	*   \param component Pointer to component
	*
	*   Computational Complexity of this routine is O(n), where n is amount of distinct component types
	*/
	bool validComponent(Entity owner, Component* component) {
		for(unsigned int i = 0; i < containers.size(); i++) {
			if(containers[i].first <= (char*)component && (char*)component <= (containers[i].first +
					containers[i].second.sizeOfComponent * containers[i].second.freeIndex)) {
				if(component->owner == owner) {
					return true;
				} else {
					return false;
				}
			}
		}
		return false;
	}

	/** \brief tests validity of component handle(pointer) if it points to location inside component container
	*       and component at this location belongs to desired owner
	*
	*   \param owner Entity that desired component belongs to
	*   \param component Pointer to component
	*
	*   Computational Complexity is O(1), because we do know component type.
	*/
	template<typename ComponentClass>
	bool validComponent(Entity owner, ComponentClass* component) {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		auto& container = containers[containerIndex];

		//in the case that delated component was alone in system and
		//system is now empty(normal check wouldn't catch this and would return that component is valid)
		if(container.second.freeIndex == 0) {
			return false;
		}

		if(container.first <= (char*)component && (char*)component <= (container.first +
				container.second.sizeOfComponent * container.second.freeIndex)) {
			if(component->owner == owner) {
				return true;
			}
		}
		return false;
	}

	/** \brief returns component of desired type that is owned by particular Entity
	*
	* \param owner Entity that desired component is part of
	*
	* ComponentClass template parameter is type of component that we want to get
	*
	* Computational Complexity is O(log(n)), where n is amount of components of the same type in system
	* Uses iterative binary search
	*
	* Returned pointer is valid only temporaly, because delete operation on container with components with the same
	*  type as desired component(ComponentClass) or reallocation of this container(in the case of expanding container)
	*  will move components to other area of memory, pointer will point to other data(other component, or garbage)
	*
	*  Validity of pointer can be checked by validComponent.
	*
	* \return pointer to desired component valid temporaly
	*/
	template<typename ComponentClass>
	ComponentClass* getComponent(Entity owner) {
		size_t containerIndex = ContainerID::value<ComponentClass>();
		auto& container = containers[containerIndex];
		if(container.first == nullptr) {
			return nullptr;
		}

		return (ComponentClass*)findComponent(owner, container);
	}

	/** \brief gives access to all components of desired type
	*
	* ComponentClass is template parameter that specifies what type of components we want
	* Computational Complexity is O(1)
	*
	* Returned structure is just pointer encapsulated in structure for convenient indexed access, with size field.
	* Structure is temporal, can invalidate when adress of component container for desired type changes(reallocation)
	* or amount of components in container changes (size field will be invalid)
	*
	* \returns temporal structure representing list of all components of desired type
	*/
	template<typename ComponentClass>
	Components<ComponentClass> getComponents() {
		size_t containerIndex = ContainerID::value<ComponentClass>();

		size_t size = containers[containerIndex].second.freeIndex;
		ComponentClass* components = (ComponentClass*)containers[containerIndex].first;
		return {size, components};
	}

	/** \brief fills containers with all component pointers that belongs to entities,
	*       which contain all of these components.
	*       It can be thinked as method that returns all Entites which have at least all specified components
	*
	*   \param head pointer to first of containers that will be filled with component pointers
	*   \param tail all other container pointers that will be filled with component pointers (any amount)
	*
	*   It will select entities that have property of having all of component types given(HeadComponentType and
	*       all types in TailComponents), and then fill given containers with pointers to particular components
	*       of these entities.
	*   All containers wil have the same amount of component pointer in these
	*   For given index i, in all containers component at this index wil belong to the same entity.
	*
	*   Computational Complexity is O(m * k * log(n)), where m is amount of distinct component types for whom we
	*   are asking(in parameters)(really it's determined on compile-time), k is amount of components of type in head,
	*   and n is amount of components in particular component type we are searching for component from the same entity
	*   as in head(recursively m times). log(n) comes from particular binary search. There is single binary search per
	*   k * m(for each component in head, check in all component types that we're asked if there is component of the
	*   same entity(if is, then add of course). k and n in practice can be simmilar, and m is hardcoded
	*   in routine call, so it's practically O(n log(n)).
	*
	*   For performance reasons, try to put most rare component types in the front arguments. This will reduce k.
	*/
	template<typename HeadComponentType, typename... TailComponents>
	void intersection(std::vector<HeadComponentType*>& head, std::vector<TailComponents*>& ... tail) {
		Components<HeadComponentType> headComponents = getComponents<HeadComponentType>();
		if(headComponents.size() == 0) {
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

	/** \brief intersection with single component type, for completness sake.
	*
	*   \param container container where all component pointers of given type will be placed
	*
	*   Computational Complexity: O(n), where n is amount of components of desired type in the system
	*
	*   It's equivalent for getComponents(), differs only in that it gives list of pointers instead of direct access.
	*/
	template<typename ComponentType>
	void intersection(std::vector<ComponentType*>& container) {
		Components<ComponentType> headComponents = getComponents<ComponentType>();
		if(headComponents.size() == 0) {
			return;
		}

		for(size_t i = 0; i < headComponents.size(); i++) {
			container.push_back(&headComponents[i]);
		}
	}

	/** \brief creates new component in the system
	*
	*   \param owner Entity that component will belong to
	*   \param args settings that will be passed to init routine of component. In the case of empty(default),
	*       init won't be called.
	*
	*   ComponentClass template parameter specifies type of component that will be created.
	*
	*   Can fail if there won't be any aviable memory left(unlikely)
	*
	* 	Computational complexity: O(n), where n is amount of components of the same type in system. Optimistic
	*   complexity is O(1), which will be really often in case(probably much more often than pessimistic).
	*
	*   Will reallocate container in the case of exceeding current capacity. This will invalidate any pointers to any
	*       of components of the same type
	*
	*   In the case of creating component to old entity cost will be searching for place - O(log(n)), and
	*       moving all components of entities more new to the right, which can be pessimisticaly O(n), and of course
	*       will invalidate all pointers pointing to that components. It involve copying things in main memory, so
	*       it's slow.
	*
	*   \returns pointer to newly created component, or nullptr in the case of failure.
	*/
	template<typename ComponentClass>
	ComponentClass* createComponent(Entity owner, ArgsMap args = ArgsMap()) {
		auto container = prepareComponentContainer<ComponentClass>();
		if(!container) {
			logger.fatal("Cannot prepare component container. Probably we don't have any memory left.");
			return nullptr;
		}

		char* adress = preparePlaceForNewComponent<ComponentClass>(owner);
		ComponentClass* createdComponent = new(adress) ComponentClass;
		createdComponent->owner = owner;
		container->second.freeIndex++;

		if(!args.empty()) {
			createdComponent->init(args);
		}

		return createdComponent;
	}

	/** \brief deletes component from the system
	*
	*   \param owner Entity that desired component belongs to
	*
	*   ComponentClass template parameter specifies type of desired component.
	*
	*  Computational Complexity: O(n), where n is amount of components of the same type as desired component.
	*  Optimistic computational complexity is O(log(n)), becuase we must just find component to delete
	*
	*  If possible, delete component of new entities first, or delete before creating new entities in the first place.
	*  This operation is slowest of all, because if we're deleting components from old entities, we must copy all
	*  more new components to the left(to fill empty space).
	*
	*  \return true if component was deleted, false otherwise (for example when there isn't such component)
	*/
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

	/** \brief deletes component from the system by direct pointer to component and without exact type
	*
	*   \param owner Entity that contains this component.
	*   \param componentToDelete poitner to component that we want to have deleted
	*
	*   Computational Complexity: O(m + n), where n is amount of components with the same type as desired component,
	*       and m is amout of distinct component types(negligable compared to n, in most cases)
	*   Cost is comparable to delete with exact type.
	*
	*   \returns true if component was deleted, false otherwise(for ex. if pointer don't point to desired component)
	*/
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

	/** \brief checks if entity is existing in system
	*
	*   \param entityID entity value to check(entity representation)
	*
	*   \returns entity state(existing/not existing) Entity is non-exsitng if wasn't created in the first place or
	*   was delated.
	*/
	bool entityExist(Entity entityID) {
		return entityID < entityExistingTable.size() && entityExistingTable[entityID];
	}

	/** \brief creates new Entity
	*
	*   \returns newly created Entity's represnetation (id)
	*/
	Entity createEntity() {
		entityExistingTable.push_back(true);
		return entityExistingTable.size() - 1;
	}

	/** \brief deletes all components that given entity is composed with, and deletes entity itself
	*
	*   \param owner Entity to delete
	*
	*   Computational Complexity: O(m * n), where m is amount of distinct component types(usually negligable), and
	*   n is amount of components of particular component type.
	*
	*   Cost is roughly the same as m * deleteComponent.
	*/
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
