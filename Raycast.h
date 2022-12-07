#pragma once
#include <glm/glm.hpp>
#include "Console.h"
#include "SceneManager.h"
#include "MeshCollisionBox.h"
#include "Engine.h"
struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
	float distance;
};
struct RayHit {
	glm::vec3 position;
	unsigned int entity;
};
bool IntersectsCollider(const Ray& ray, const MeshCollisionBox& collider, float* distance = nullptr, int entity = -1) {
	if (glm::distance(ray.origin, collider.transform->position) > ray.distance)
		return false;
	float minX = (collider.min.x - ray.origin.x) / ray.direction.x;
	float maxX = (collider.max.x - ray.origin.x) / ray.direction.x;
	float minY = (collider.min.y - ray.origin.y) / ray.direction.y;
	float maxY = (collider.max.y - ray.origin.y) / ray.direction.y;
	float minZ = (collider.min.z - ray.origin.z) / ray.direction.z;
	float maxZ = (collider.max.z - ray.origin.z) / ray.direction.z;
	
	float min = std::max(std::max(std::min(minX, maxX), std::min(minY, maxY)), std::min(minZ, maxZ));
	float max = std::min(std::min(std::max(minX, maxX), std::max(minY, maxY)), std::max(minZ, maxZ));

	if (max < 0.0f)
		return false;

	if (min > max)
		return false;
	
	if (min < 0.0f && distance != nullptr) {
		*distance = max;
		return true;
	}

	*distance = min;

	return true;
}

bool CheckCollision(Ray& ray, std::vector<RayHit>* outHit, int excludeEntity = -1) {
	for (auto collider : Engine::FindComponentsInScene<MeshCollisionBox>()) {
		auto& meshCollider = Engine::GetComponent<MeshCollisionBox>(collider);
		float distance;
		if (IntersectsCollider(ray, meshCollider, &distance, collider) && collider != excludeEntity) {
			outHit->push_back({ ray.origin + (ray.direction * distance), collider });
		}
	}
	return outHit->size() > 0;
}


bool Raycast(glm::vec3 origin, glm::vec3 direction, float distance, std::vector<RayHit>* outHits = nullptr, int excludeEntity = -1) {
	Ray ray = { origin, direction, distance };
	return CheckCollision(ray, outHits, excludeEntity);
}