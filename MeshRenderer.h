#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Shaders.h"
#include "Engine.h"
#include "Transform.h"
#include <string>
#include "ModelLoader.h"
class MeshRenderer : public Component {
public:
	char modelPath[128] = "";
    std::string lastFramePath;
	Mesh mesh;
    bool hasModel = false;
	MeshRenderer(){
        this->lastFramePath = modelPath;
        mesh = ModelLoader::LoadModel(modelPath);
        hasModel = mesh.vertices.size() != 0;
	}
	void Update() {
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
        if (std::string(modelPath) != lastFramePath) {
            mesh = ModelLoader::LoadModel(std::string(modelPath));
            hasModel = mesh.vertices.size() != 0;
            lastFramePath = std::string(modelPath);
            std::cout << hasModel << std::endl;
        }
	}
};