#pragma once
#include "MeshCollisionBox.h"
struct Collision {
public:
    Box a;
    Box b;
    glm::vec3& velocityA;
    glm::vec3 normal;
    float time;
};
// Note - Must fix to allow the multiple meshes in the collision boxes (probably later)
class Collisions {
private:

    static void ResolveCollision(std::vector<Collision> collisions);
    static void CheckCollisions(std::vector<Collision>* collisions, unsigned int e, MeshCollisionBox& box);
    static std::vector<Collision> FindCollision();
    static bool Collides(glm::vec3 positionA, glm::vec3 positionB, MeshCollisionBox a, MeshCollisionBox b);
    static bool CheckSweptCollision(Box a, Box b, glm::vec3& velocityA, float* outTime, glm::vec3* outNormal);
public:
    static void HandleCollision();
};