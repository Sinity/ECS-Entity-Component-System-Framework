#include "componentManager.h"
#include "entityManager.h"

using namespace EECS;

void EECS::ComponentManager::setEntityManager(const EntityManager& entityManager) {
    this->entityManager = &entityManager;
}

bool EECS::ComponentManager::entityExists(EntityID entity) {
    if (entityManager) {
        return entityManager->entityExists(entity);
    }

    return true; // no checking if entity manager isn't set
}
