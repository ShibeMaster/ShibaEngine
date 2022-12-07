#pragma once
#include "BoundingBox.h"
#include "ModelLoader.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "ProjectItem.h"
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
    void Update(bool inRuntime) {
        if (debugDraw) {
            glm::mat4 model = transform->GetMatrix();
            Shaders::activeShader.SetMat4("model", model);
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
            std::cout << box.meshItem.name << " || " << box.meshItem.path << std::endl;
            box.LoadMeshBox();
        }
		ImGui::Checkbox("Debug Draw", &box.debugDraw);
	}
    void GetObject(ClassInstance* instance) {
    }
    void SetObject(ClassInstance* instance) {
    }
};