#pragma once

#include <GL\glew.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Game {
public:
	int width = 1366;
	int height = 768;
	int antiAliasing = 4;
	float fov = 70;
	bool fullscreen = false;
	GLFWwindow* window;
};

class ShaderCompiler
{
public:
	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};
