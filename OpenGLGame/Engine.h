#pragma once

#include <GL\glew.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

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
	void RenderWireFrame();
	GLuint shaderProgram;
public:
	bool opaque; // Should the model be opaque or see through?
	std::vector<glm::vec3> vertices;
	glm::vec3 color;
	Model(const char * path, glm::vec3 color, GLuint shaderProgram) : Model(path, color, shaderProgram, true) {};
	Model(const char * path, glm::vec3 color, GLuint shaderProgram, bool opaque);
	~Model();
	void Render(GLuint shaderProgram);
};

class Time {
public:
	static float getDelta();
	static void Update();
};

class Terrain
{
	int sizeX, sizeY;
	GLfloat density;
public:
	std::vector<glm::vec3> vertices;
	Terrain(GLuint shaderProgram);
	~Terrain();
	void Render();
};
