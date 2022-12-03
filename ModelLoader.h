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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class ModelLoader {
public:
	static Model LoadModel(const std::string& path) {
		Model model;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "loading model failed with error " << importer.GetErrorString() << std::endl;
			return Model();
		}

		auto directory = path.substr(0, path.find_last_of('\\'));

		model.directory = directory;
		ProcessNode(&model, scene->mRootNode, scene, directory);
		return model;
	}
	static Sprite LoadSprite(const std::string& path) {
		Sprite sprite;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &sprite.width, &sprite.height, &sprite.channels, 0);
		float aspectRatioHeight = ((float)sprite.height / (float)sprite.width)/2;
		float aspectRatioWidth = ((float)sprite.width / (float)sprite.height)/2;
		std::vector<Vertex> vertices = {	
			{ glm::vec3(aspectRatioWidth, aspectRatioHeight, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f)},
			{ glm::vec3(aspectRatioWidth, -aspectRatioHeight, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f)},
			{ glm::vec3(-aspectRatioWidth, -aspectRatioHeight, 0.0f), glm::vec2(0.0f), glm::vec3(0.0f)},
			{ glm::vec3(-aspectRatioWidth, aspectRatioHeight, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f) }
		};
		unsigned int texture1;
		std::cout << aspectRatioWidth << " || " << aspectRatioHeight << std::endl;
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Texture tex = { texture1, "", path };
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, 0x1908, sprite.width, sprite.height, 0, 0x1908, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			sprite.mesh = Mesh(vertices, { 0, 1, 3, 1, 2, 3 }, { tex });
			sprite.texture1 = texture1;
		}
		else {
			std::cout << "loading sprite failed" << std::endl;
		}
		stbi_image_free(data);
		return sprite;

	}
	static void ProcessNode(Model* model, aiNode* node, const aiScene* scene, const std::string& directory) {

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			LoadMesh(mesh, model, scene, directory);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(model, node->mChildren[i], scene, directory);
		}
	}
	static void LoadMesh(aiMesh* mesh, Model* model, const aiScene* scene, const std::string& directory) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		bool hasMaterial = false;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			glm::vec3 value;
			value.x = mesh->mVertices[i].x;
			value.y = mesh->mVertices[i].y;
			value.z = mesh->mVertices[i].z;
			vertex.position = value;
			if (mesh->HasNormals()) {
				value.x = mesh->mNormals[i].x;
				value.y = mesh->mNormals[i].y;
				value.z = mesh->mNormals[i].z;
				vertex.normal = value;
			}
			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.textureCoords = vec;

				if (mesh->HasTangentsAndBitangents()) {

					value.x = mesh->mTangents[i].x;
					value.y = mesh->mTangents[i].y;
					value.z = mesh->mTangents[i].z;
					vertex.tangent = value;
					value.x = mesh->mBitangents[i].x;
					value.y = mesh->mBitangents[i].y;
					value.z = mesh->mBitangents[i].z;
					vertex.bitangent = value;
				}

			}
			else
				vertex.textureCoords = glm::vec2(0.0f);

			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int e = 0; e < face.mNumIndices; e++)
				indices.push_back(face.mIndices[e]);
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> textures;

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		
		std::vector<Texture> normalMaps = LoadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = LoadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


		auto meshVal = Mesh(vertices, indices, textures);
		meshVal.mesh = mesh;
		model->meshes.push_back(meshVal);

	}
	static std::vector<Texture> LoadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int e = 0; e < model->textures.size(); e++) {
				if (std::strcmp(model->textures[e].path.data(), str.C_Str()) == 0) {
					textures.push_back(model->textures[e]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = GenerateTexture(str.C_Str(), model->directory);
				if (texture.id != -1) {
					texture.type = typeName;
					texture.path = str.C_Str();
					model->textures.push_back(texture);
				}
			}
		}
		return textures;
	}
	static int GenerateTexture(const char* path, const std::string& directory) {
		auto filename = std::string(path);
		filename = directory + "\\" + filename;

		int width, height, comps;
		std::cout << filename << std::endl;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &comps, 0);
		if (data) {
			unsigned int id;
			glGenTextures(1, &id);

			GLenum format;
			if (comps == 1)
				format = GL_RED;
			else if (comps == 3)
				format = GL_RGB;
			else if (comps == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "failed to load image" << std::endl;
		}
		
		stbi_image_free(data);
		return -1;
	}

};