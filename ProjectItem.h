#pragma once
#include <string>
enum class ProjectItemType {
	none,
	folder,
	script,
	model
};
class ProjectItem {
public:
	std::string name;
	std::string path;
	std::string type;
};