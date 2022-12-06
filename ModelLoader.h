#pragma once
#include "Mesh.h"
#include <iostream>
#include <string>
#include <vector>
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <GL/GL.h>
#include <assimp/postprocess.h>
#include "Sprite.h"

class ModelLoader {
public:
	static Model LoadModel(const std::string& path);
	static unsigned int LoadCubeMap(std::vector<std::string> sides);
	static Sprite LoadSprite(const std::string& path);
	static void ProcessNode(Model* model, aiNode* node, const aiScene* scene, const std::string& directory);
	static void LoadMesh(aiMesh* mesh, Model* model, const aiScene* scene, const std::string& directory);
	static std::vector<Texture> LoadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type, std::string typeName);
	static int GenerateTexture(const char* path, const std::string& directory);

};