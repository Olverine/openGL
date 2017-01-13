#include "Vehicle.h"

float currentVelocity = 0;

void Vehicle::Init() {
	position = glm::vec3(0, 256, 0);
}

void Vehicle::Update() {
	float turnInput = 0;
	if (Input::GetKey(GLFW_KEY_LEFT)) {
		turnInput++;
	}
	if (Input::GetKey(GLFW_KEY_RIGHT)) {
		turnInput--;
	}

	rotation = glm::vec3(Lerp(rotation.x, glm::radians(turnInput * bankAngle), bankSpeed), rotation.y + glm::radians((rotation.x / glm::radians(bankAngle)) * angularVelocity), 0);

	if (Input::GetKey(GLFW_KEY_UP)) {
		currentVelocity = Lerp(currentVelocity, velocity, acceleration);
	}
	else if (Input::GetKey(GLFW_KEY_DOWN)) {
		currentVelocity -= brakePower;
		if (currentVelocity < 0)
			currentVelocity = 0;
	}
	else {
		currentVelocity = Lerp(currentVelocity, 0, acceleration / 2);
	}

	position += GetHorizontalForward() * currentVelocity;

	// Clamp the position of the vehicle to keep it inside the terrain
	if (position.x > terrain->physicalSize.x / 2) {
		position.x = terrain->physicalSize.x / 2;
	}
	else if (position.z > terrain->physicalSize.y / 2) {
		position.z = terrain->physicalSize.y / 2;
	}
	if (position.x < -terrain->physicalSize.x / 2) {
		position.x = -terrain->physicalSize.x / 2;
	}
	else if (position.z < -terrain->physicalSize.y / 2) {
		position.z = -terrain->physicalSize.y / 2;
	}

	float y = terrain->GetElevation(position.x, position.z);
	if (position.y < y) {
		position.y = y;
	}
	else {
		position.y = Lerp(position.y, y, 0.1f);
	}
}