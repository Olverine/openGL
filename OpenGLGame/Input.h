#pragma once
#include <Engine.h>

class Input
{
	// position
	glm::vec3 position = glm::vec3(0, 0, 5);
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.01f;

public:
	Input();
	~Input();

	static glm::mat4 GetProjectionMatrix(float fov);
	static glm::mat4 GetViewMatrix();
	static void ComputeMatricesFromInput(GLFWwindow* window);
};

