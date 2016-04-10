#include "entity.h"

namespace EECS {

Entity EntityManager::getEntity(EntityID entityID) { return {entityID, *this, componentManager}; }

Entity EntityManager::addEntity() {
    entityExistence[++lastEntity] = true;
    return {lastEntity, *this, componentManager};
}

Entity EntityManager::cloneEntity(EntityID source) {
    if (!entityExists(source)) {
        return {0, *this, componentManager};
    }

    Entity target = addEntity();

    for (auto& container : componentManager.containers) {
        if (container) {
            container->cloneComponent(source, target);
        }
    }

    return target;
}

bool EntityManager::deleteEntity(EntityID entityID) {
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

void EntityManager::clear() {
    for (auto entityEntry : entityExistence) {
        deleteEntity(entityEntry.first);
    }

    entityExistence.clear();
}
}
