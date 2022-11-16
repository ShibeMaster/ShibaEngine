#pragma once
#include <vector>
#include "Transform.h"
#include <string>
class Scene {
public:
	std::string name;
	std::vector<unsigned int> entities;
};