#include "EntityManager.h"

EntityManager::EntityManager() {
	for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
		entityIdQueue.push(i);
	}
}
unsigned int EntityManager::CreateEntity() {
	unsigned int newEntityId = entityIdQueue.front();
	entityIdQueue.pop();
	existingEntities++;
	return newEntityId;
}
void EntityManager::DestroyEntity(unsigned int entity) {
	entityIdQueue.push(entity);
	existingEntities--;
}