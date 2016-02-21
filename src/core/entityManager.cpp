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
}
