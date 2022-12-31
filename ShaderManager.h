#pragma once
#include "Shader.h"
#include "ShaderInstanceData.h"
#include "SerializationUtils.h"
#include <unordered_map>
/// <summary>
/// This class is going to be the main thing that is going to be managing all of our shaders and allowing us to quickly switch between all of them.
/// </summary>
class ShaderManager {
public:
	// Just gonna write out a quite explanation here about the shader system that I'm going to implement into the engine.
	// The engine is going to support custom user written glsl shaders and also supplies a few default ones
	// Every shader (including the ones that are going to be written by users) will be able to access a few uniforms that are going to be set and managed by the engine each frame:
	// - view (mat4)
	// - projection (mat4)
	// - model (mat4)
	// - hasTexture (bool)
	// - viewPos (vec3)
	// - lightPos (vec3)
	// - lightColour (vec3)
	// - lightEffected (bool)
	// You don't exactly need to implement these, but they're here if you even need them
	// To access textures you need to make a uniform with the sampler2D type and the name of: "texture_" + (the texture type) + (the texture number)
	// The layout of the shaders will remain consistant throughout all of them:
	// (position) | (type) | (name)
	// 0 | vec3 | position
	// 1 | vec3 | normal
	// 2 | vec3 | texCoords
	




	static Shader* shader;
	static std::unordered_map<std::string, Shader> shaders;

	static void ApplyUniforms(const std::string& shader, ShaderUniformData* uniforms) {
		for (auto& uniform : ShaderManager::shaders[shader].uniforms) {
			switch (uniform.type)
			{
			case GL_FLOAT: ShaderManager::shader->SetFloat(uniform.name, uniforms->floats[uniform.name]); break;
			case GL_FLOAT_VEC4: ShaderManager::shader->SetVec4(uniform.name, uniforms->vec4s[uniform.name]); break;
			case GL_FLOAT_VEC3: ShaderManager::shader->SetVec3(uniform.name, uniforms->vec3s[uniform.name]); break;
			case GL_FLOAT_VEC2: ShaderManager::shader->SetVec2(uniform.name, uniforms->vec2s[uniform.name]); break;
			case GL_BOOL: ShaderManager::shader->SetBool(uniform.name, uniforms->bools[uniform.name]); break;
			case GL_INT: ShaderManager::shader->SetInt(uniform.name, uniforms->ints[uniform.name]); break;
			default:
				break;
			}
		}
	}
	static void SetShader(const std::string& str) {
		shader = &shaders[str];
		shader->Use();
	}
	static void AddShader(const std::string& str, const Shader& shader, const std::string& type = "") {
		shaders[str] = shader;
		shaders[str].type = type;
	}
	static void AddShader(const std::string& str, const char* vertex, const char* fragment, const std::string& type = "") {
		shaders[str] = Shader(vertex, fragment);
		shaders[str].type = type;
	}
	static void RemoveCustomShaders() {
		std::vector<const char*> removingShaders;
		for (auto& shader : shaders) {
			if (shader.first.find("ShibaEngine_") == std::string::npos)
				removingShaders.push_back(shader.first.c_str());
		}
		for (auto& shader : removingShaders)
			shaders.erase(shader);
	}
	static void LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& type = "") {
		auto vertex = SerializationUtils::ReadFile(vertexPath);
		auto fragment = SerializationUtils::ReadFile(fragmentPath);
		AddShader(name, vertex.c_str(), fragment.c_str(), type);
	}
	static void LoadDefaultShaders() {
		LoadShader("ShibaEngine_Default", "default_vertex.txt", "default_fragment.txt", "Model");
		LoadShader("ShibaEngine_Billboard", "billboard_vertex.txt", "billboard_fragment.txt", "Sprite");
		LoadShader("ShibaEngine_Skybox", "skybox_vertex.txt", "skybox_fragment.txt", "Skybox");
		LoadShader("ShibaEngine_Sprite", "sprite_default_vertex.txt", "sprite_default_fragment.txt", "Sprite");
	}
};