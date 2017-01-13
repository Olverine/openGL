#include "Engine.h"

GLFWwindow* window;

bool Input::GetKey(int key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void Input::SetWindow(GLFWwindow* w) {
	window = w;
}