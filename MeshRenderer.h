#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Shaders.h"
#include "Engine.h"
#include "Transform.h"
#include "Scripting.h"
#include "GUIExtensions.h"
#include <string>
#include "ProjectItem.h"
#include "ModelLoader.h"
class MeshRenderer : public Component {
public:
    ProjectItem modelItem = ProjectItem { "Model Item" };
	Mesh mesh;
    bool hasModel = false;
	MeshRenderer(){
        ReloadMesh();
	}
    static void DrawGUI(unsigned int selectedEntity) {
        if (Engine::HasComponent<MeshRenderer>(selectedEntity)) {
            bool meshRendererExists = true;
            if (ImGui::CollapsingHeader("Mesh Renderer", &meshRendererExists)) {
                MeshRenderer& renderer = Engine::GetComponent<MeshRenderer>(selectedEntity);
                ImGui::Button(renderer.modelItem.name.c_str());
                ProjectItem item;
                if (GUIExtensions::CreateProjectItemDropField(".fbx", &item)) {
                    renderer.modelItem = item;
                    std::cout << renderer.modelItem.name << " || " << renderer.modelItem.path << std::endl;
                    renderer.ReloadMesh();
                }
            }
            if (!meshRendererExists)
                Engine::RemoveComponent<MeshRenderer>(selectedEntity);

        }
    }
    void ReloadMesh() {
        mesh = ModelLoader::LoadModel(modelItem.path);
        hasModel = mesh.vertices.size() != 0;
    }
	void Update(bool inRuntime) {
        if (hasModel) {

            auto transform = Engine::GetComponent<Transform>(entity);
            glm::mat4 model = transform.GetMatrix();
            Shaders::activeShader.SetMat4("model", model);

            /*unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            for (unsigned int i = 0; i < mesh.textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = mesh.textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++);

                Shaders::activeShader.SetInt(("material." + name + number).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
            }
            glActiveTexture(GL_TEXTURE0);*/

            mesh.Render();
        }
	}
    void GetObject(ClassInstance* instance) {
        instance->SetFieldValue<MonoString>("modelPath", mono_string_new(Scripting::data.appDomain, modelItem.path.c_str()));
    }
    void SetObject(ClassInstance* instance) {
        modelItem = ProjectManager::activeProject.GetItem(mono_string_to_utf8(instance->GetFieldValue<MonoString*>("modelPath")));
        ReloadMesh();
    }
};