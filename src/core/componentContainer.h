#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include "entityID.h"

namespace EECS {

// Base of all component containers, for operations which need to be done without knowing exact type of container.
class ComponentContainerBase {
   public:
    virtual ~ComponentContainerBase() {}

    virtual void clear() = 0;
    virtual std::unique_ptr<ComponentContainerBase> getNewClassInstance() const = 0;

    virtual bool cloneComponent(EntityID sourceEntity, EntityID recipientEntity) = 0;
    virtual bool genericDeleteComponent(EntityID entity) = 0;
};

// Template class used for storing components of particular type.
template <class T>
class ComponentContainer : public ComponentContainerBase {
   public:
    // returns pointer to a component owned by given entity, in O(lg n). nullptr if component doesn't exist.
    T* getComponent(EntityID entityID) {
        auto componentIt =
            std::lower_bound(components.begin(), components.end(), entityID,
                             [](const T& component, EntityID entityID) { return component.entityID < entityID; });

        if (componentIt == components.end() || componentIt->entityID != entityID) {
            return nullptr;
        }

        return &*componentIt;
    }

    // Returns all components held by this class. It's fast method, through dangerous. User shouldn't modify
    // the vector in any way, otherwise class invariants could be invalidated. It's not const vector because then
    // modifying components itself would be impossible, which would render this method useless. If user wants to
    // batch process every/most of components, it's much faster than getting them one by one with getComponent. If user
    // don't know exact entity id, then it's only viable method to do so.
    std::vector<T>& getAllComponents() { return components; }

    // adds new component, replaces existing component if already exists. Arguments after EntityID will be passed
    // directly to component's constructor. Returns pointer to created component.
    template <typename... Args>
    T* addComponent(EntityID entityID, Args&&... args) {
        if (entityID == 0) {
            return nullptr;
        }

        auto place =
            std::lower_bound(components.begin(), components.end(), entityID,
                             [](const T& component, EntityID entityID) { return component.entityID < entityID; });

        auto componentAlreadyExists = place != components.end() && place->entityID == entityID;
        if (componentAlreadyExists) {
            *place = T(std::forward<Args>(args)...);
        } else {
            place = components.insert(place, T(std::forward<Args>(args)...));
        }

        place->entityID = entityID;
        return &*place;
    }

    // copies component from one entity to another. Returns true if component was cloned, otherwise false.
    bool cloneComponent(EntityID sourceEntity, EntityID recipientEntity) override {
        if (sourceEntity == 0 || recipientEntity == 0) {
            return false;
        }

        auto sourceComponent = getComponent(sourceEntity);
        if (!sourceComponent) {
            return false;
        }

        auto targetComponent = addComponent(recipientEntity);
        if (!targetComponent) {
            return false;
        }

        *targetComponent = *sourceComponent;
        targetComponent->entityID = recipientEntity;

        return true;
    }

    // Deletes component of given Entity. Returns true if deleted, false if it doesn't exist in the first place.
    bool deleteComponent(EntityID entityID) {
        auto componentIt =
            std::lower_bound(components.begin(), components.end(), entityID,
                             [](const T& component, EntityID entityID) { return component.entityID < entityID; });

        if (componentIt != components.end() && componentIt->entityID == entityID) {
            components.erase(componentIt);
            return true;
        }

        return false;
    }

    // used internally as a method to delete all components from given entity.
    bool genericDeleteComponent(EntityID entityID) override { return deleteComponent(entityID); }

    // Deletes all components
    void clear() override { components.clear(); }

    // returns new object of the same class as *this*.
    std::unique_ptr<ComponentContainerBase> getNewClassInstance() const override {
        return std::make_unique<ComponentContainer<T>>();
    }

   private:
    std::vector<T> components;
};
}
