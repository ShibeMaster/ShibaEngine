#pragma once
#include "MeshCollisionBox.h"
struct Collision {
public:
    unsigned int a;
    unsigned int b;
    MeshCollisionBox& bbA;
    MeshCollisionBox& bbB;

    bool operator==(Collision rhs) {
        return a == rhs.a && b == rhs.b;
    }
};
// Note - Must fix to allow the multiple meshes in the collision boxes (probably later)
class Collisions {
private:

    static void ResolveCollision(std::vector<Collision> collisions);
    static void CheckCollisions(std::vector<Collision>* collisions, unsigned int e, MeshCollisionBox& box);
    static std::vector<Collision> FindCollision();
    static bool Collides(glm::vec3 positionA, glm::vec3 positionB, MeshCollisionBox a, MeshCollisionBox b);
public:
    static void HandleCollision();
};