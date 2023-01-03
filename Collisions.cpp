#include "Collisions.h"
#include "Physics.h"
#include "Engine.h"
#include "Scene.h"
#include <limits>

void Collisions::ResolveCollision(std::vector<Collision> collisions) {
    for (auto& col : collisions) {
        glm::vec3 vel = col.velocityA * Time::deltaTime;
        float remainingTime = 1.0f - col.time;
        float magnitude = glm::length(vel * Time::deltaTime) * remainingTime;
        float dot = glm::dot(vel * Time::deltaTime, col.normal);
        if (dot > 0.0f) dot = 1.0f;
        else if (dot < 0.0f) dot = -1.0f;

        col.velocityA = glm::vec3(dot * col.normal * magnitude) / Time::deltaTime;
        col.velocityA.y = 0.0f;
    }
}
void Collisions::CheckCollisions(std::vector<Collision>* collisions, unsigned int e, MeshCollisionBox& box) {
    Transform& transform = Engine::GetComponent<Transform>(e);
    auto& physics = Engine::GetComponent<Physics>(e);
    Box boxA = box.GetBox();
    for (auto& otherComp : Engine::FindComponentsInScene<MeshCollisionBox>()) {
        if (e != otherComp) {
            auto& otherTransform = Engine::GetComponent<Transform>(otherComp);
            auto& otherBoundingBox = Engine::GetComponent<MeshCollisionBox>(otherComp);
            float time;
            glm::vec3 normal;
            Box boxB = otherBoundingBox.GetBox();
            if (ShouldCheckSwept(boxA, physics.velocity, boxB)) {
                if (CheckSweptCollision(boxA, boxB, physics.velocity, &time, &normal)) {
                    collisions->push_back({ boxA, boxB, physics.velocity, normal, time });
                }
            }
        }
    }
}
std::vector<Collision> Collisions::FindCollision() {

    auto boundingBoxes = Engine::FindComponentsInScene<MeshCollisionBox>();
    std::vector<Collision> collisions;
    for (auto comp : boundingBoxes) {
        if (Engine::HasComponent<Physics>(comp)) {
            auto& boundingBox = Engine::GetComponent<MeshCollisionBox>(comp);
            CheckCollisions(&collisions, comp, boundingBox);
        }
    }

    return collisions;
}
bool Collisions::Collides(Box a, Box b) {

    return (
        a.min.x <= b.max.x &&
        a.max.x >= b.min.x &&
        a.min.y <= b.max.y &&
        a.max.y >= b.min.y &&
        a.min.z <= b.max.z &&
        a.max.z >= b.min.z);
}
bool Collisions::Collides(Box2d a, Box2d b) {

    return (
        a.min.x <= b.max.x &&
        a.max.x >= b.min.x &&
        a.min.y <= b.max.y &&
        a.max.y >= b.min.y);
}
bool Collisions::ShouldCheckSwept(Box a, glm::vec3 velocity, Box b) {
    Box aWhole;
    aWhole.min.x = velocity.x > 0.0f ? a.min.x : a.min.x + velocity.x;
    aWhole.min.y = velocity.y > 0.0f ? a.min.y : a.min.y + velocity.y;
    aWhole.min.z = velocity.z > 0.0f ? a.min.z : a.min.z + velocity.z;
    aWhole.max.x = velocity.x > 0.0f ? a.max.x + velocity.x : a.max.x;
    aWhole.max.y = velocity.y > 0.0f ? a.max.y + velocity.y : a.max.y;
    aWhole.max.z = velocity.z > 0.0f ? a.max.z + velocity.z : a.max.z;
    return Collides(aWhole, b);
}
bool Collisions::CheckSweptCollision(Box a, Box b, glm::vec3& velocity, float* outTime, glm::vec3* normal) {
    glm::vec3 velocityA = velocity * Time::deltaTime;
    glm::vec3 invEntry;
    glm::vec3 invExit;

    invEntry.x = b.min.x - a.max.x;
    invExit.x = b.max.x - a.min.x;
    if (!velocityA.x > 0.0f)
        std::swap(invEntry.x, invExit.x);
    
    invEntry.y = b.max.y - a.min.y;
    invExit.y = b.min.y - a.max.y;
    if (!velocityA.y > 0.0f)
        std::swap(invEntry.y, invExit.y);

    invEntry.z = b.min.z - a.max.z;
    invExit.z = b.min.z - a.min.z;
    if (!velocityA.z > 0.0f)
        std::swap(invEntry.z, invExit.z);

    glm::vec3 entry, exit;
    entry.x = velocityA.x != 0.0f ? invEntry.x / velocityA.x : -std::numeric_limits<float>::infinity();
    exit.x = velocityA.x != 0.0f ? invExit.x / velocityA.x : std::numeric_limits<float>::infinity();
    entry.y = velocityA.y != 0.0f ? invEntry.y / velocityA.y : -std::numeric_limits<float>::infinity();
    exit.y = velocityA.x != 0.0f ? invExit.y / velocityA.y : std::numeric_limits<float>::infinity();
    entry.z = velocityA.z != 0.0f ? invEntry.z / velocityA.z : -std::numeric_limits<float>::infinity();
    exit.z = velocityA.z != 0.0f ? invExit.z / velocityA.z : std::numeric_limits<float>::infinity();
    float entryTime = std::max(entry.x, std::max(entry.y, entry.z));
    float exitTime = std::min(exit.x, std::max(exit.y, exit.z));
    if (entryTime > exitTime || (entry.x < 0.0f && entry.y < 0.0f) || entry.x > 1.0f || entry.y > 1.0f)
        return false;

    if (std::max(entry.x, std::max(entry.y, entry.z)) == entry.x)
        *normal = entry.x < 0.0f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
    else if (std::max(entry.x, std::max(entry.y, entry.z)) == entry.y)
        *normal = entry.y < 0.0f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
    else
        *normal = entry.z < 0.0f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
    *outTime = entryTime;
    return true;

}
void Collisions::HandleCollision() {
    auto collisions = FindCollision();
    ResolveCollision(collisions);
}