#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include "entityID.h"

struct Component;

// Base of all component containers, for generic operations and cloning the container.
class ComponentContainerBase {
public:
    virtual ~ComponentContainerBase() {
    }

    // Used when user don't know exact type of this class(have only ptr to the base).
    virtual Component* genericGetComponent(EntityID entityID) = 0;
    virtual Component* genericAddComponent(EntityID entityID) = 0;
    virtual bool genericDeleteComponent(EntityID entityID) = 0;
    virtual Component* cloneComponent(EntityID sourceEntity, EntityID recipientEntity) = 0;
    virtual void clear() = 0;

    virtual std::unique_ptr<ComponentContainerBase> clone() const = 0;
};

// Template class used for storing components of particular type.
template<class T>
class ComponentContainer : public ComponentContainerBase {
public:
    // returns pointer to component owned by given entity, in O(lg n). nullptr if component don't exist.
    T* getComponent(EntityID entityID) {
        auto componentIt = std::lower_bound(components.begin(), components.end(), entityID,
                                            [](const T& component, EntityID entityID) {
                                                return component.entityID < entityID;
                                            });

        if (componentIt == components.end() || componentIt->entityID != entityID) {
            return nullptr;
        }

        return &*componentIt;
    }

    // Returns all components held by this class. It's fast method, through dangerous. User shouldn't modify
    // the vector in any way, otherwise class invariants could be invalidated. It's not const vector because then
    // modifying components itself would be impossible, which would render this method useless. If user wants to
    // batch process every/most of components, it's much faster than getting them one by one with getComponent. If user
    // don't know exact EntityID's, then it's only viable method to do so.
    std::vector<T>& getAllComponents() {
        return components;
    }

    // adds new component, replaces existing component if already exists. Arguments after EntityID will be passed
    // directly to component's constructor. Returns pointer to created component.
    template<typename... Args>
    T* addComponent(EntityID entityID, Args&& ... args) {
        if (entityID == 0) {
            return nullptr;
        }

        auto place = std::lower_bound(components.begin(), components.end(), entityID,
                                      [](const T& component, EntityID entityID) {
                                          return component.entityID < entityID;
                                      });

        auto componentAlreadyExists = place != components.end() && place->entityID == entityID;
        if (componentAlreadyExists) {
            *place = T(std::forward<Args>(args)...);
        } else {
            place = components.insert(place, T(std::forward<Args>(args)...));
        }

        place->entityID = entityID;
        return &*place;
    }

    Component* cloneComponent(EntityID sourceEntity, EntityID recipientEntity) override {
        if(sourceEntity == 0 || recipientEntity == 0) {
            return nullptr;
        }

        auto sourceComponent = getComponent(sourceEntity);
        if(!sourceComponent) {
            return nullptr;
        }

        auto targetComponent = addComponent(recipientEntity);
        if(!targetComponent) {
            return nullptr;
        }

        *targetComponent = *sourceComponent;
        targetComponent->entityID = recipientEntity;

        return (Component*)targetComponent;
    }

    // Deletes component of given Entity. Returns true if deleted, false if it doesn't exist in the first place.
    bool deleteComponent(EntityID entityID) {
        auto componentIt = std::lower_bound(components.begin(), components.end(), entityID,
                                            [](const T& component, EntityID entityID) {
                                                return component.entityID < entityID;
                                            });

        if (componentIt != components.end() && componentIt->entityID == entityID) {
            components.erase(componentIt);
            return true;
        }

        return false;
    }

    // Deletes all components
    void clear() override {
        components.clear();
    }

    // Returns pointer to component which belong to given Entity.
    Component* genericGetComponent(EntityID entityID) override {
        return getComponent(entityID);
    }

    // Adds component to given entity, using default constructor.
    Component* genericAddComponent(EntityID entityID) override {
        return addComponent(entityID);
    }

    // Deletes component which belong to given Entity
    bool genericDeleteComponent(EntityID entityID) override {
        return deleteComponent(entityID);
    }

    // Clones this object and returns the clone
    std::unique_ptr<ComponentContainerBase> clone() const override {
        std::unique_ptr<ComponentContainer<T>> thisClone = std::make_unique<ComponentContainer<T>>();
        thisClone->components = components;
        return std::move(thisClone);
    }

private:
    std::vector<T> components;
};
