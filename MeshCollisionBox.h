#pragma once
#include "BoundingBox.h"
#include "ModelLoader.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "ProjectItem.h"
class MeshCollisionBox : public BoundingBox {
public:
    Mesh mesh;
    ProjectItem meshItem = { "meshItem" };
    void LoadMeshBox() {
        glm::vec3 minBox = glm::vec3(0.0f);
        glm::vec3 maxBox = glm::vec3(0.0f);
        for (auto vertex : ModelLoader::LoadModel(meshItem.path).vertices) {
            maxBox.x = std::max(maxBox.x, vertex.position.x);
            minBox.x = std::min(minBox.x, vertex.position.x);

            maxBox.y = std::max(maxBox.y, vertex.position.y);
            minBox.y = std::min(minBox.y, vertex.position.y);

            maxBox.z = std::max(maxBox.z, vertex.position.z);
            minBox.z = std::min(minBox.z, vertex.position.z);
        }
        max = maxBox;
        min = minBox;
        std::vector<unsigned int> indices;
        for (int i = 0; i < 36; i++) {
            indices.push_back(i);
        }
        debugMesh = Mesh({
                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f)},

                Vertex {glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f)},
                Vertex {glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f)},
            }, indices);
    }
	static void DrawGUI(unsigned int selectedEntity) {
		if (Engine::HasComponent<MeshCollisionBox>(selectedEntity)) {
			bool boundingBoxExists = true;
			if (ImGui::CollapsingHeader("Mesh Collision", &boundingBoxExists)) {
				MeshCollisionBox& box = Engine::GetComponent<MeshCollisionBox>(selectedEntity);
                ImGui::Button(box.meshItem.name.c_str());
                ProjectItem item;
                if (GUIExtensions::CreateProjectItemDropField(".fbx", &item)) {
                    box.meshItem = item;
                    std::cout << box.meshItem.name << " || " << box.meshItem.path << std::endl;
                    box.LoadMeshBox();
                }
				ImGui::Checkbox("Debug Draw", &box.debugDraw);
                ImGui::DragFloat3("Min", &box.min[0], 0.1f);
                ImGui::DragFloat3("Max", &box.max[0], 0.1f);
			}

            if (!boundingBoxExists) {
                Engine::RemoveComponent<MeshCollisionBox>(selectedEntity);
                Engine::RemoveComponent<BoundingBox>(selectedEntity);
            }
		}
	}
    void GetObject(ClassInstance* instance) {
        instance->SetFieldValue<glm::vec3>("min", min);
        instance->SetFieldValue<glm::vec3>("max", max);
    }
    void SetObject(ClassInstance* instance) {
        min = instance->GetFieldValue<glm::vec3>("min");
        max = instance->GetFieldValue<glm::vec3>("max");
    }
};