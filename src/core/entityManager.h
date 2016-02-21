#pragma once
#include <unordered_map>
#include "componentManager.h"

namespace EECS {
class Entity;

class EntityManager {
   public:
    explicit EntityManager(ComponentManager& componentManager) : componentManager(componentManager) {}

    bool entityExists(EntityID entityID) const { return entityExistence.find(entityID) != entityExistence.end(); }

    Entity getEntity(EntityID entityID);

    Entity addEntity();

    Entity cloneEntity(EntityID source);

    bool deleteEntity(EntityID entityID) {
        if (entityID == 0) {
            return false;
        }

        auto it = entityExistence.find(entityID);
        if (it == entityExistence.end()) {
            return false;
        }

        for (auto& container : componentManager.containers) {
            container->genericDeleteComponent(entityID);
        }

        entityExistence.erase(it);
        return true;
    }

    void clear() {
        for (auto entityEntry : entityExistence) {
            deleteEntity(entityEntry.first);
        }

        entityExistence.clear();
    }

   private:
    std::unordered_map<EntityID, bool> entityExistence;
    EntityID lastEntity = 0;
    ComponentManager& componentManager;
};
}
