#pragma once
#include <queue>
#define MAX_ENTITIES 4096

class EntityManager {
private:
	std::queue<unsigned int> entityIdQueue;
public:
	int existingEntities = 0;
	EntityManager();
	unsigned int CreateEntity();
	void DestroyEntity(unsigned int entity);
};