#pragma once
#include "Engine.h"
class Vehicle : public Mesh
{
	float currentVelocity = 0;
public:
	Terrain* terrain;
	float velocity;
	float acceleration;
	float brakePower;
	float angularVelocity;
	float bankAngle;
	float bankSpeed;
	float bank;
	int joystick;
	bool joystickPresent;

	Vehicle(const char * path, glm::vec3 color, GLuint shaderProgram, int joystick) : Mesh(path, color, shaderProgram, true) {
		velocity = 300.0f;
		angularVelocity = 1.0f;
		brakePower = 1;
		bankAngle = 20;
		bankSpeed = 0.1f;
		acceleration = 0.001f;
		this->joystick = joystick;
	};

	void Init();
	void Update();
};