#pragma once
#include "ProjectItem.h"
#include <string>
#include "Scripting.h"
#include "ProjectManager.h"
#include "imgui.h"

class GUIExtensions {
public:
	template<typename T>
	static bool CreateDragDropTarget(const std::string& targetType, T* outValue) {

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string(std::string("DRAG_DROP_") + targetType).c_str())) {
				*outValue = *(T*)payload->Data;
				return true;
			}
		}
		return false;
	}
	static bool CreateProjectItemDropField(std::vector<std::string> fileTypes, ProjectItem* outItem) {

		if (ImGui::BeginDragDropTarget()) {
			for (auto& type : fileTypes) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string("DRAG_DROP_" + type).c_str())) {
					auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data);
					*outItem = script;
					return true;
				}
			}
		}
		return false;
	}
	static void CreateInstanceDragDropField(Field field, ClassInstance& instance) {
		MonoObject* entityValue = instance.GetFieldValue<MonoObject*>(field.name);
		unsigned int entity;
		if (entityValue != nullptr) {
			mono_field_get_value(entityValue, Scripting::instanceClass.fields["entity"].classField, &entity);
			ImGui::Button(SceneManager::activeScene->items[entity].name.c_str());
		}
		else {
			ImGui::Button("Undefined Instance");
		}
		if (GUIExtensions::CreateDragDropTarget<unsigned int>("Entity", &entity)) {
			instance.SetFieldValue<MonoObject>(field.name, Scripting::data.entities[entity].instance.instance);
		}
	}
	static void RenderField(Field field, ClassInstance& instance) {
		// im sorrry for making this
		float floatVal;
		double doubleVal;
		bool boolVal;
		std::string stringVal;
		glm::vec2 vec2Val;
		glm::vec3 vec3Val;
		int intValue;
		switch (field.type) {
		case FieldType::Float:
			floatVal = instance.GetFieldValue<float>(field.name);
			ImGui::InputFloat(field.name.c_str(), &floatVal);
			if (floatVal != instance.GetFieldValue<float>(field.name))
				instance.SetFieldValue<float>(field.name, floatVal);
			break;
		case FieldType::Double:
			doubleVal = instance.GetFieldValue<double>(field.name);
			ImGui::InputDouble(field.name.c_str(), &doubleVal);
			if (doubleVal != instance.GetFieldValue<double>(field.name))
				instance.SetFieldValue<double>(field.name, doubleVal);
			break;
		case FieldType::Bool:
			boolVal = instance.GetFieldValue<bool>(field.name);
			ImGui::Checkbox(field.name.c_str(), &boolVal);
			if (boolVal != instance.GetFieldValue<bool>(field.name))
				instance.SetFieldValue<bool>(field.name, boolVal);
			break;
		case FieldType::Instance:
			CreateInstanceDragDropField(field, instance);
			break;
		case FieldType::Vector2:
			vec2Val = instance.GetFieldValue<glm::vec2>(field.name);
			ImGui::InputFloat2(field.name.c_str(), &vec2Val[0]);
			if (vec2Val != instance.GetFieldValue<glm::vec2>(field.name))
				instance.SetFieldValue<glm::vec2>(field.name, vec2Val);
			break;
		case FieldType::Vector3:
			vec3Val = instance.GetFieldValue<glm::vec3>(field.name);
			ImGui::InputFloat3(field.name.c_str(), &vec3Val[0]);
			if (vec3Val != instance.GetFieldValue<glm::vec3>(field.name))
				instance.SetFieldValue<glm::vec3>(field.name, vec3Val);
			break;
		case FieldType::Int:
			intValue = instance.GetFieldValue<int>(field.name);
			ImGui::InputInt(field.name.c_str(), &intValue);
			if (intValue != instance.GetFieldValue<int>(field.name.c_str()))
				instance.SetFieldValue<int>(field.name, intValue);
			break;

		}
	}
	static void RenderShaderUniforms(const std::string& shader, ShaderUniformData* uniforms) {
		for (auto& uniform : ShaderManager::shaders[shader].uniforms) {
			switch (uniform.type)
			{
			case GL_FLOAT: ImGui::InputFloat(uniform.name.c_str(), &uniforms->floats[uniform.name]); break;
			case GL_FLOAT_VEC4: ImGui::DragFloat4(uniform.name.c_str(), &uniforms->vec4s[uniform.name][0], 0.05f); break;
			case GL_FLOAT_VEC3: ImGui::DragFloat3(uniform.name.c_str(), &uniforms->vec3s[uniform.name][0], 0.05f); break;
			case GL_FLOAT_VEC2: ImGui::DragFloat2(uniform.name.c_str(), &uniforms->vec2s[uniform.name][0], 0.05f); break;
			case GL_BOOL: ImGui::Checkbox(uniform.name.c_str(), &uniforms->bools[uniform.name]); break;
			case GL_INT: ImGui::DragInt(uniform.name.c_str(), &uniforms->ints[uniform.name], 0.05f); break;
			default:
				break;
			}
		}
	}

};