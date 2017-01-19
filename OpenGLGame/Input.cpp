#include "Engine.h"

bool Input::GetKey(int key) {
	return glfwGetKey(GetWindow(), key) == GLFW_PRESS;
}