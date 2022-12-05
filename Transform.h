#pragma once
#include <glm/glm.hpp>
#include "ScriptingTypes.h"
#include <glm/gtc/matrix_transform.hpp>
class Transform	 {
public:
	void Start(){}
	void Update(bool inRuntime){}
	void Initialize(){}
	static void DrawGUI(unsigned int selectedEntity){}
	void SetObject(ClassInstance* instance) {
		position = instance->GetFieldValue<glm::vec3>("position");
		rotation = instance->GetFieldValue<glm::vec3>("rotation");
		pivot = instance->GetFieldValue<glm::vec3>("pivot");
		scale = instance->GetFieldValue<glm::vec3>("scale");
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<glm::vec3>("position", position);
		instance->SetFieldValue<glm::vec3>("rotation", rotation);
		instance->SetFieldValue<glm::vec3>("pivot", pivot);
		instance->SetFieldValue<glm::vec3>("scale", scale);
	}

	unsigned int entity;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 pivot = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 pivot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f)) {
		this->position = position;
		this->rotation = rotation;
		this->pivot = pivot;
		this->scale = scale;
	}
	glm::mat4 GetMatrix() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);

		model = glm::translate(model, pivot);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, -pivot);
		model = glm::scale(model, scale);
		return model;
	}
};