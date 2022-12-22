#pragma once
#include <glm/glm.hpp>
#include "imgui.h"
class EditorFrame {
public:
	bool isOpen = true;
	virtual void Render() = 0;
};