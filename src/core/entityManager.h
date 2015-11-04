#pragma once
#include <unordered_map>
#include "entity.h"

class EntityManager {
public:
    explicit EntityManager(ComponentManager& componentManager) : componentManager(componentManager) {
    }

    bool entityExists(EntityID entityID) const {
        return entityExistance.find(entityID) != entityExistance.end();
    }

    Entity getEntity(EntityID entityID) {
        return {entityID, componentManager};
    }

    Entity addEntity() {
        entityExistance[++lastEntity] = true;
        return {lastEntity, componentManager};
    }

    Entity cloneEntity(EntityID source) {
        if(!entityExists(source)) {
            return {0, componentManager};
        }

        Entity target = addEntity();

        for(auto& container : componentManager.containers) {
            if(container) {
               container->cloneComponent(source, target); 
            }
        }

        return target;
    }

    void deleteEntity(EntityID entityID) {
        if (entityID == 0) {
            return;
        }

        auto it = entityExistance.find(entityID);
        if(it == entityExistance.end()) {
            return;
        }

        for (auto& container : componentManager.containers) {
            container->genericDeleteComponent(entityID);
        }

        entityExistance.erase(it);
    }

private:
    std::unordered_map<EntityID, bool>  entityExistance;
    EntityID lastEntity = 0;
    ComponentManager& componentManager;
};
