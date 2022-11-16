#pragma once
#include <GLFW/glfw3.h>
#include "Mouse.h"
class InputManager {
public:
	static Mouse mouse;

	static void SetMouseUnlocked() {
		mouse.inputLocked = false;
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	static void SetMouseLocked() {
		mouse.inputLocked = true;
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	static bool GetKeyDown(int key) {
		return glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
	}
	static float MoveVert() {
		bool forward = GetKeyDown(GLFW_KEY_W);
		bool backward = GetKeyDown(GLFW_KEY_S);
		return forward && !backward ? 1.0f : backward && !forward ? -1.0f : 0.0f;
	}
	static float MoveHorz() {
		bool forward = GetKeyDown(GLFW_KEY_D);
		bool backward = GetKeyDown(GLFW_KEY_A);
		return forward && !backward ? 1.0f : backward && !forward ? -1.0f : 0.0f;
	}
	static float MoveUpDown() {

		bool forward = GetKeyDown(GLFW_KEY_E);
		bool backward = GetKeyDown(GLFW_KEY_Q);
		return forward && !backward ? 1.0f : backward && !forward ? -1.0f : 0.0f;
	}
	static bool IsMovingAny() {
		return MoveHorz() != 0.0f || MoveVert() != 0.0f || MoveUpDown() != 0.0f;
	}
};