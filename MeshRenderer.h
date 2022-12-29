#pragma once
#include "Mesh.h"
#include "Component.h"
#include "ShaderManager.h"
#include "ShaderInstanceData.h"
#include "Engine.h"
#include "Transform.h"
#include "Scripting.h"
#include "GUIExtensions.h"
#include "imgui_stdlib.h"
#include <string>
#include "ProjectItem.h"
#include "Renderer.h"
#include "ModelLoader.h"
class MeshRenderer : public Component {
public:
    ProjectItem modelItem = ProjectItem { "Model Item" };
    std::string shader = "ShibaEngine_Default";
    ShaderUniformData uniforms;
	Model model;
    bool hasModel = false;
	MeshRenderer(){
        ReloadMesh();
	}
    static void DrawGUI(unsigned int selectedEntity) {
        MeshRenderer& renderer = Engine::GetComponent<MeshRenderer>(selectedEntity);
        ImGui::Button(renderer.modelItem.name.c_str());
        ProjectItem item;
        if (GUIExtensions::CreateProjectItemDropField({ ".fbx", ".obj" }, &item)) {
            renderer.modelItem = item;
            renderer.ReloadMesh();
        }
        ImGui::InputText("Shader", &renderer.shader);
        if (ShaderManager::shaders[renderer.shader].uniforms.size() > 0) {
            ImGui::Text("Uniforms");
            GUIExtensions::RenderShaderUniforms(renderer.shader, &renderer.uniforms);
        }
    }
    void ReloadMesh() {
        model = ModelLoader::LoadModel(modelItem.path);
        hasModel = model.meshes.size() != 0;
    }
	void Render() {
        if (hasModel) {
            Renderer::ChangeShader(shader);

            glm::mat4 modelMat = transform->GetMatrix();
            Renderer::SetModel(modelMat);
            ShaderManager::ApplyUniforms(shader, &uniforms);


            for (auto& mesh : model.meshes)
                mesh.Render();
        }
	}
    void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
        json->Key("Model");
        json->String(modelItem.path.c_str());
        json->Key("Shader");
        json->String(shader.c_str());
        json->Key("Uniforms");
        json->StartArray();
        SerializeUniforms(json);
        json->EndArray();
    }
    // a terrible way of doing this, but it still works
    void SerializeUniforms(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
        for (auto bools : uniforms.bools) {
            json->StartObject();
            SerializationUtils::SerializeUniformInformation(json, "bool", bools.first);
            json->Key("value");
            json->Bool(bools.second);
            json->EndObject();
        }
        for (auto floats : uniforms.floats) {
            json->StartObject();
            SerializationUtils::SerializeUniformInformation(json, "float", floats.first);
            json->Key("value");
            json->Double((double)floats.second);
            json->EndObject();
        }
        for (auto vec3s : uniforms.vec3s) {
            json->StartObject();
            SerializationUtils::SerializeUniformInformation(json, "vec3", vec3s.first);
            SerializationUtils::SerializeVec3("value", vec3s.second, json);
            json->EndObject();
        }
    }
    void DeserializeUniforms(rapidjson::Value& obj) {
        for (auto& uniform : obj["Uniforms"].GetArray()) {
            std::string type = uniform["type"].GetString();
            std::string name = uniform["name"].GetString();
            if (type == "bool")
                uniforms.bools[name] = uniform["value"].GetBool();
            if (type == "float")
                uniforms.floats[name] = (float)uniform["value"].GetDouble();
            if (type == "vec3")
                uniforms.vec3s[name] = SerializationUtils::DeserializeVec3(uniform["value"]);

        }
    }
    void Deserialize(rapidjson::Value& obj) {
        modelItem = ProjectManager::activeProject.GetItem(obj["Model"].GetString());
        shader = obj["Shader"].GetString();
        ReloadMesh();
        DeserializeUniforms(obj);
    }
    void GetObject(ClassInstance* instance) {
        instance->SetFieldValue<MonoString>("modelPath", mono_string_new(Scripting::data.appDomain, modelItem.path.c_str()));
        instance->SetFieldValue<MonoString>("shader", mono_string_new(Scripting::data.appDomain, modelItem.path.c_str()));
    }
    void SetObject(ClassInstance* instance) {
        modelItem = ProjectManager::activeProject.GetItem(mono_string_to_utf8(instance->GetFieldValue<MonoString*>("modelPath")));
        shader = mono_string_to_utf8(instance->GetFieldValue<MonoString*>("shader"));
        ReloadMesh();
    }
};