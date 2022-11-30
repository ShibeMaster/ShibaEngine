#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Component.h"
struct BoundingBox {
public:
	glm::vec3 min = glm::vec3(-0.5f);
	glm::vec3 max = glm::vec3(0.5f);
};