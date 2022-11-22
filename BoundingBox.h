#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Component.h"
class BoundingBox : public Component {
public:
	glm::vec3 min = glm::vec3(-0.5f);
	glm::vec3 max = glm::vec3(0.5f);
	bool debugDraw = false;
	Mesh debugMesh;

	void Update(bool inRuntime) {
        if (debugDraw) {
            auto transform = Engine::GetComponent<Transform>(entity);
            glm::mat4 model = transform.GetMatrix();
            Shaders::activeShader.SetMat4("model", model);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            debugMesh.Render();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
	}
};