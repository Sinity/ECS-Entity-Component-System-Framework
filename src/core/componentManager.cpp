#include "componentManager.h"
#include "entityManager.h"

using namespace EECS;

void ComponentManager::setEntityManager(const EntityManager& entityManager) { this->entityManager = &entityManager; }

bool ComponentManager::entityExists(EntityID entity) {
    if (entityManager) {
        return entityManager->entityExists(entity);
    }

    return true;  // no checking if entity manager isn't set
}
