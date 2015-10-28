#include "componentsManager.h"
#include "entityManager.h"

void ComponentsManager::setEntityManager(const EntityManager& entityManager) {
    this->entityManager = &entityManager;
}

bool ComponentsManager::entityExists(EntityID entity) {
    if (entityManager) {
        return entityManager->entityExists(entity);
    }

    return true; // no checking if entity manager isn't set
}
