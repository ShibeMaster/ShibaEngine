#pragma once
#include "Mesh.h"
#include <iostream>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader {
public:
	static Mesh LoadModel(const std::string& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "loading model failed with error " << importer.GetErrorString() << std::endl;
			return Mesh();
		}

		auto directory = path.substr(0, path.find_last_of('/'));

		std::vector<Mesh> meshes;
		ProcessNode(&meshes, scene->mRootNode, scene, directory);
		return meshes[0];
	}
	static void ProcessNode(std::vector<Mesh>* meshes, aiNode* node, const aiScene* scene, const std::string& directory) {

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes->push_back(LoadMesh(mesh, scene, directory));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(meshes, node->mChildren[i], scene, directory);
		}
	}
	static Mesh LoadMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

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

			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int e = 0; e < face.mNumIndices; e++)
				indices.push_back(face.mIndices[e]);
		}

		return Mesh(vertices, indices);

	}
};