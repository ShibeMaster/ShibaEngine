#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Shaders.h"
#include "Engine.h"
#include "Transform.h"
class MeshRenderer : public Component {
public:
	Mesh mesh;
	MeshRenderer(){}
	MeshRenderer(Mesh mesh) {
		this->mesh = mesh;
	}
	void Update() {
		auto transform = Engine::GetComponent<Transform>(entity);
		glm::mat4 model = transform.GetMatrix();
		Shaders::activeShader.SetMat4("model", model);
		mesh.Render();
	}
};