#pragma once
#include "Engine.h"
class Vehicle : public Mesh
{
public:
	Terrain* terrain;
	float velocity;
	float acceleration;
	float brakePower;
	float angularVelocity;
	float bankAngle;
	float bankSpeed;
	int joystick;
	bool joystickPresent;

	Vehicle(const char * path, glm::vec3 color, GLuint shaderProgram) : Mesh(path, color, shaderProgram, true) {
		velocity = 300.0f;
		angularVelocity = 1.0f;
		brakePower = 1;
		bankAngle = 20;
		bankSpeed = 0.1f;
		acceleration = 0.001f;
		joystick = GLFW_JOYSTICK_1;
	};

	void Init();
	void Update();
};