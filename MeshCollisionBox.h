#pragma once
#include "BoundingBox.h"
#include "ModelLoader.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "ProjectItem.h"
// must fix to allow multiple boundingBoxes
class MeshCollisionBox : public Component {
public:
    Model model;
    std::vector<BoundingBox> boundingBoxes;
    ProjectItem meshItem = { "meshItem" };
    bool debugDraw;
    std::vector<Mesh> debugMeshes;
    glm::vec3 max;
    glm::vec3 min; 
    // these are the overall max and min positions of all of the mesh bounding boxes
    // it'll be used to find whether it should check if each of the boxes collide
    void LoadMeshBox() {
        model = ModelLoader::LoadModel(meshItem.path);
        
        for (auto& mesh : model.meshes) {
            BoundingBox box;
            for (auto& vertex : mesh.vertices) {

                box.max.x = std::max(box.max.x, vertex.position.x);
                box.min.x = std::min(box.min.x, vertex.position.x);

                box.max.y = std::max(box.max.y, vertex.position.y);
                box.min.y = std::min(box.min.y, vertex.position.y);

                box.max.z = std::max(box.max.z, vertex.position.z);
                box.min.z = std::min(box.min.z, vertex.position.z);
            }
            max.x = std::max(box.max.x, max.x);
            min.x = std::min(box.min.x, min.x);
            max.y = std::max(box.max.y, max.y);
            min.y = std::min(box.min.y, min.y);
            max.z = std::max(box.max.z, max.z);
            min.z = std::min(box.min.z, min.z);

            boundingBoxes.push_back(box);
        }

        for (auto& box : boundingBoxes) {
            debugMeshes.push_back(GenerateBoundingBoxDebugMesh(box));
        }
        
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
            for (auto& mesh : debugMeshes) {
                mesh.Render();
            }
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