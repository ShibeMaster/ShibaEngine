#pragma once
#include <GLFW/glfw3.h>
class Display {
public:
	static GLFWwindow* window;
	static int width;
	static int height;

	static GLFWwindow* CreateWindow() {
		window = glfwCreateWindow(width, height, "Shiba Engine", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSetWindowSizeCallback(window, Display::HandleWindowResize);
		return window;
	}
	static void ShowWindow() {
		glfwShowWindow(window);
	}
	static void HandleWindowResize(GLFWwindow* window, int width, int height) {
		Display::width = width;
		Display::height = height;
	}
};