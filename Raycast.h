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

bool IntersectsCollider(const Ray& ray, const MeshCollisionBox& collider) {
	if (glm::distance(ray.origin, Engine::GetComponent<Transform>(collider.entity).position) > ray.distance)
		return false;
	float intersectMinX = (collider.min.x - ray.origin.x) / ray.direction.x;
	float intersectMaxX = (collider.max.x - ray.origin.x) / ray.direction.x;

	if (intersectMinX > intersectMaxX) std::swap(intersectMinX, intersectMaxX);

	float intersectMinY = (collider.min.y - ray.origin.y) / ray.direction.y;
	float intersectMaxY = (collider.max.y - ray.origin.y) / ray.direction.y;

	if (intersectMinY > intersectMaxY) std::swap(intersectMinY, intersectMaxY);


	float tMin = std::max(intersectMinX, intersectMinY);
	float tMax = std::min(intersectMaxX, intersectMaxY);

	if (intersectMinX > intersectMaxY || intersectMinY > intersectMaxX)
		return false;
	
	if (intersectMinY > intersectMinX)
		intersectMinX = intersectMinY;


	if (intersectMaxY < intersectMaxX)
		intersectMaxX = intersectMaxY;
	

	float intersectMinZ = (collider.min.z - ray.origin.z) / ray.direction.z;
	float intersectMaxZ = (collider.max.z - ray.origin.z) / ray.direction.z;

	if (intersectMinZ > intersectMaxZ) std::swap(intersectMinZ, intersectMaxZ);

	if (intersectMinX > intersectMaxZ || intersectMinZ < intersectMaxX)
		return false;

	if (intersectMinZ > intersectMinX)
		intersectMinX = intersectMinZ;

	if (intersectMaxZ < intersectMaxX)
		intersectMaxX = intersectMaxZ;

	return true;

}

bool CheckCollision(Ray& ray, std::vector<unsigned int>* outHit, int excludeEntity) {
	for (auto collider : Engine::FindComponentsInScene<MeshCollisionBox>()) {
		if (IntersectsCollider(ray, Engine::GetComponent<MeshCollisionBox>(collider)) && collider != excludeEntity) {
			outHit->push_back(collider);
		}
	}
	return outHit->size() > 0;
}


bool Raycast(glm::vec3 origin, glm::vec3 direction, float distance, std::vector<unsigned int>* outHits = nullptr, int excludeEntity = -1) {
	Ray ray = { origin, direction, distance };
	return CheckCollision(ray, outHits, excludeEntity);
}