#pragma once
#include "ShaderManager.h"
#include <queue>
struct ShaderDataBuffer {
	glm::mat4 view;
	glm::mat4 projection;

	glm::vec3 viewPos;
	glm::vec3 lightPos;
	glm::vec3 lightColour;
};
class Renderer {
private:
	static void ApplyShaderUniforms() {
		ShaderManager::shader->SetMat4("view", shaderData.view);
		ShaderManager::shader->SetMat4("projection", shaderData.projection);
		ShaderManager::shader->SetVec3("viewPos", shaderData.viewPos);
		ShaderManager::shader->SetVec3("lightPos", shaderData.lightPos);
		ShaderManager::shader->SetVec3("lightColour", shaderData.lightColour);
	}
public:
	static ShaderDataBuffer shaderData;

	static void ChangeShader(const std::string& shader) {
		ShaderManager::SetShader(shader);
		ApplyShaderUniforms();
	}
	static void SetModel(const glm::mat4& model) {
		ShaderManager::shader->SetMat4("model", model);
	}
};