#pragma once
#include "BoundingBox.h"
#include "MeshCollisionBox.h"
#include "Engine.h"
#include "Scene.h"
#include "Console.h"
struct Collision {
public:
    unsigned int a;
    unsigned int b;

    bool operator==(Collision rhs) {
        return a == rhs.a && b == rhs.b;
    }
};
class Collisions {
public:
    static void HandleCollision() {
        auto collisions = FindCollision();
        ResolveCollision(collisions);
    }
    static void ResolveCollision(std::vector<Collision> collisions) {
        for (auto comp : collisions) {
            glm::vec3& aPosition = Engine::GetComponent<Transform>(comp.a).position;
            glm::vec3& bPosition = Engine::GetComponent<Transform>(comp.b).position;

            if (Engine::HasComponent<Physics>(comp.a)) {
                Physics& aPhysics = Engine::GetComponent<Physics>(comp.a);
                aPosition -= aPhysics.velocity * Time::deltaTime;
            }
            if (Engine::HasComponent<Physics>(comp.b)) {
                Physics& bPhysics = Engine::GetComponent<Physics>(comp.b);
                bPosition -= bPhysics.velocity * Time::deltaTime;
            }
        }
    }
    static void CheckCollisions(std::vector<Collision>* collisions, unsigned int e, MeshCollisionBox box) {
        Transform& transform = Engine::GetComponent<Transform>(e);
        for (auto otherComp : Engine::FindComponentsInScene<MeshCollisionBox>()) {
            if (e != otherComp) {
                auto otherTransform = Engine::GetComponent<Transform>(otherComp);
                auto otherBoundingBox = Engine::GetComponent<MeshCollisionBox>(otherComp);
                if (Collides(transform.position, otherTransform.position, box, otherBoundingBox)) {
                    if (std::find(collisions->begin(), collisions->end(), Collision{ otherComp, e }) == collisions->end()) {
                        Console::LogMessage("collided");
                        collisions->push_back({ e, otherComp });
                    }
                }
            }
        }
    }
    static std::vector<Collision> FindCollision() {

        auto boundingBoxes = Engine::FindComponentsInScene<MeshCollisionBox>();
        std::vector<Collision> collisions;
        for (auto comp : boundingBoxes) {
            auto boundingBox = Engine::GetComponent<MeshCollisionBox>(comp);
            CheckCollisions(&collisions, comp, boundingBox);
        }

        return collisions;
    }
	static bool Collides(glm::vec3 positionA, glm::vec3 positionB, MeshCollisionBox a, MeshCollisionBox b) {
        a.min += positionA;
        a.max += positionA;
        b.min += positionB;
        b.max += positionB;
        return (
            a.min.x <= b.max.x &&
            a.max.x >= b.min.x &&
            a.min.y <= b.max.y &&
            a.max.y >= b.min.y &&
            a.min.z <= b.max.y &&
            a.max.z >= b.min.z);
	}
};