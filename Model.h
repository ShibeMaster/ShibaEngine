#pragma once
#include <vector>
#include "Mesh.h"
class Model {
public:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::string directory;
};