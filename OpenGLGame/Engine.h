#pragma once

#include <GL\glew.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

float Lerp(float x, float y, float a);
//void DrawLine(glm::vec3 p1, glm::vec3 p2);
GLuint GetShaderProgram();

int GetWidth();
int GetHeight();
GLFWwindow* GetWindow();
float GetAspectRatio();

int GetPlayerCount();

class ShaderCompiler {
public:
	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};

class GameObject
{
protected:
	// vertex buffer identity
	GLuint vertexbuffer;
	// id of color in shader
	GLuint colorID;
public:
	GameObject();
	~GameObject();

	std::vector<glm::vec3> vertices;

	// Transformation vectors
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);

	void SetColor(glm::vec3 color);

	virtual void Init();
	virtual void Update();
	virtual void Render();
};

GameObject* GetPlayer(int index);

class Mesh : public GameObject {
	void RenderWireFrame();
	GLuint shaderProgram;
public:
	bool opaque; // Should the model be opaque or see through?
	glm::vec3 color;

	Mesh(const char * path, glm::vec3 color, GLuint shaderProgram) : Mesh(path, color, shaderProgram, true) {};
	Mesh(const char * path, glm::vec3 color, GLuint shaderProgram, bool opaque);
	~Mesh();

	void Render();

	glm::vec3 GetHorizontalForward();
};

class Camera
{
	// Initial Field of View
	float initialFoV = 60.0f;

	float speed = 1; // 3 units / second
	float mouseSpeed = 0.01f;

public:
	Camera();
	~Camera();

	virtual void Update();

	// position
	glm::vec3 position = glm::vec3(60, 10, 0);
	// horizontal angle : toward -Z
	float horizontalAngle = 0;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;

	glm::mat4 GetProjectionMatrix(float fov);
	glm::mat4 GetViewMatrix();
	static void ComputeMatricesFromInput(GLFWwindow* window);
};

class Time {
public:
	static float getDelta();
	static void Update();
};

class Terrain : public GameObject
{
	int sizeX, sizeY;
	GLfloat density;
	float heightScale;
public:
	glm::vec2 physicalSize;
	Terrain(GLuint shaderProgram, unsigned char* data);
	~Terrain();
	void Render();

	float GetElevation(float x, float y);
	glm::vec3 GetNormal(float x, float y);
};

class Input
{
public:
	static bool GetKey(int key);
};