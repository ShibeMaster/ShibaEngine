#pragma once
#include "BoundingBox.h"
#include "ModelLoader.h"
#include "MeshRenderer.h"
#include "ScriptingTypes.h"
#include "Engine.h"
#include "ProjectItem.h"
struct Box {
    glm::vec3 min;
    glm::vec3 max;
};
struct Box2d {
    glm::vec2 min;
    glm::vec2 max;
};
class MeshCollisionBox : public Component {
public:
    Model model;
    ProjectItem meshItem = { "meshItem" };
    bool debugDraw;
    glm::vec3 max;
    glm::vec3 min;
    Mesh debugMesh;
    // (Note) it's going to be more effecient in terms of performance to only have a singular mesh collider rather than several boxes
    void LoadMeshBox() {
        model = ModelLoader::LoadModel(meshItem.path);
        
        for (auto& mesh : model.meshes) {
            for (auto& vertex : mesh.vertices) {

                max.x = std::max(max.x, vertex.position.x);
                min.x = std::min(min.x, vertex.position.x);

                max.y = std::max(max.y, vertex.position.y);
                min.y = std::min(min.y, vertex.position.y);

                max.z = std::max(max.z, vertex.position.z);
                min.z = std::min(min.z, vertex.position.z);
            }
        }
        debugMesh = GenerateBoundingBoxDebugMesh({ min, max });

        
    }
    Box GetBox() {
        glm::vec3 max = this->max * transform->scale;
        glm::vec3 min = this->min * transform->scale;
        max += transform->position;
        min += transform->position;
        return { min, max };
    }
    static Mesh GenerateBoundingBoxDebugMesh(const BoundingBox& box) {
        std::vector<unsigned int> indices;
        for (int i = 0; i < 36; i++) {
            indices.push_back(i);
        }
        return Mesh({
                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(box.min.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(box.min.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.min.y, box.min.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(box.min.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.max.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(box.min.x, box.max.y, box.min.z), glm::vec3(0.0f)},
            }, indices);
    }
    void Render() {
        if (debugDraw) {
            glm::mat4 model = transform->GetMatrix();
            ShaderManager::shader->SetMat4("model", model);
            ShaderManager::shader->SetBool("lightEffected", true);
            ShaderManager::shader->SetBool("hasTexture", false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            debugMesh.Render();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
	static void DrawGUI(unsigned int selectedEntity) {
		MeshCollisionBox& box = Engine::GetComponent<MeshCollisionBox>(selectedEntity);
        ImGui::Button(box.meshItem.name.c_str());
        ProjectItem item;
        if (GUIExtensions::CreateProjectItemDropField({ ".fbx", ".obj" }, &item)) {
            box.meshItem = item;
            box.LoadMeshBox();
        }
		ImGui::Checkbox("Debug Draw", &box.debugDraw);
	}
    void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
        json->Key("Mesh");
        json->String(std::filesystem::proximate(meshItem.path, ProjectManager::activeProject.settings.directory).string().c_str());
        json->Key("Debug Draw");
        json->Bool(debugDraw);
    }
    void Deserialize(rapidjson::Value& obj) {

        if (ProjectManager::activeProject.settings.inEngine)
            meshItem = ProjectManager::activeProject.GetItem(ProjectManager::activeProject.settings.directory + obj["Mesh"].GetString());
        else
            meshItem.path = ProjectManager::activeProject.settings.directory + obj["Mesh"].GetString();
        debugDraw = obj["Debug Draw"].GetBool();
        LoadMeshBox();
    }
    void GetObject(ClassInstance* instance) {
        instance->SetFieldValue<MonoString>("meshPath", mono_string_new(Scripting::data.appDomain, meshItem.path.c_str()));
        instance->SetFieldValue<bool>("debugDraw", debugDraw);
    }
    void SetObject(ClassInstance* instance) {
        meshItem = ProjectManager::activeProject.GetItem(mono_string_to_utf8(instance->GetFieldValue<MonoString*>("meshPath")));
        debugDraw = instance->GetFieldValue<bool>("debugDraw");
        LoadMeshBox();
        
    }
};