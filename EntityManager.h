#pragma once
#include <queue>
#define MAX_ENTITIES 4096

class EntityManager {
private:
	std::queue<unsigned int> entityIdQueue;
public:
	int existingEntities = 0;
	EntityManager() {
		for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
			entityIdQueue.push(i);
		}
	}
	unsigned int CreateEntity() {
		unsigned int newEntityId = entityIdQueue.front();
		entityIdQueue.pop();
		existingEntities++;
		return newEntityId;
	}
	void DestroyEntity(unsigned int entity) {
		entityIdQueue.push(entity);
		existingEntities--;
	}
};