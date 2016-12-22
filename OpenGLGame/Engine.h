#pragma once

#include <GL\glew.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Game {
public:
	static const int width = 1366;
	static const int height = 768;
	static const int antiAliasing = 4;
	static const bool fullscreen = false;
	GLFWwindow* window;
};

class ShaderCompiler {
public:
	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};

class Model {
public:
	Model(const char * model_file_path);
	~Model();
};

class Time {
public:
	static float getDelta();
	static void Update();
};
