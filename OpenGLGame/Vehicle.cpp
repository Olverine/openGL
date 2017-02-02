#include "Vehicle.h"
#include "Fence.h"
#include <cmath>

void Vehicle::Init() {
	position = glm::vec3(rand() % 1000 + 1, 256, rand() % 1000 + 1);
}

void Vehicle::Update() {
	// Check presence of joystick
	joystickPresent = glfwJoystickPresent(joystick) == 1;

	// Get joystick input values
	int count;
	const float *axes = glfwGetJoystickAxes(joystick, &count);
	const unsigned char *buttons = glfwGetJoystickButtons(joystick, &count);

	// Get turn input from keyboard
	float turnInput = 0;
	if (Input::GetKey(GLFW_KEY_LEFT)) {
		turnInput++;
	}
	if (Input::GetKey(GLFW_KEY_RIGHT)) {
		turnInput--;
	}
	// Get turn inout from joystick
	if(joystickPresent)
		turnInput -= axes[0];
	// Clamp turn input between -1 and 1
	if (turnInput < -1) {
		turnInput = -1;
	}
	else if (turnInput > 1) {
		turnInput = 1;
	}

	// Get thottle input from keyboard
	float throttleInput = 0;
	if (Input::GetKey(GLFW_KEY_UP)) {
		throttleInput++;
	}
	// Get throttle input from joystick
	if (joystickPresent)
		throttleInput += (axes[5] + 1) / 2; // Convert input from -1.0 - 1.0 to 0.0 - 1.0
	// Clamp input
	if (throttleInput < 0) {
		throttleInput = 0;
	}
	else if (throttleInput > 1) {
		throttleInput = 1;
	}

	// Get brake input from keyboard
	float brakeInput = 0;
	if (Input::GetKey(GLFW_KEY_DOWN)) {
		brakeInput++;
	}
	// Get brake input from joystick
	if (joystickPresent)
		brakeInput = (axes[4] + 1) / 2;
	// Clamp input
	if (brakeInput < 0) {
		brakeInput = 0;
	}
	else if (brakeInput > 1) {
		brakeInput = 1;
	}

	//bank = Lerp(bank, bankAngle * turnInput, bankSpeed);

	// Get surface normal from terrain
	glm::vec3 norm = terrain->GetNormal(position.x, position.z);
	//printf("%f,%f,%f\n",norm.x, norm.y, norm.z);

	//rotation.y += glm::radians((bankAngle) * angularVelocity);
	//rotation.z = Lerp(rotation.z, atan2(norm.x, norm.y), 0.01f);
	//rotation.x = Lerp(rotation.x, -(atan2(norm.z, sqrt(pow(norm.x, 2) + pow(norm.y, 2)))),0.1f);
	
	rotation = glm::vec3(Lerp(rotation.x, glm::radians(turnInput * bankAngle), bankSpeed), rotation.y + (rotation.x / glm::radians(bankAngle) * glm::radians(angularVelocity)), 0);

	currentVelocity = Lerp(currentVelocity, throttleInput * velocity, acceleration);

	currentVelocity -= brakePower * brakeInput;
	if (currentVelocity < 0)
		currentVelocity = 0;

	position += GetHorizontalForward() * currentVelocity;

	// Clamp the position of the vehicle to keep it inside the terrain
	if (position.x > Fence::GetSize().x / 2) {
		position.x = Fence::GetSize().x / 2;
		currentVelocity /= 1.1f;
	}
	else if (position.z > Fence::GetSize().y / 2) {
		position.z = Fence::GetSize().y / 2;
		currentVelocity /= 1.1f;
	}
	if (position.x < -Fence::GetSize().x / 2) {
		position.x = -Fence::GetSize().x / 2;
		currentVelocity /= 1.1f;
	}
	else if (position.z < -Fence::GetSize().y / 2) {
		position.z = -Fence::GetSize().y / 2;
		currentVelocity /= 1.1f;
	}

	// Get elevation from terrain
	float y = terrain->GetElevation(position.x, position.z) - 50;
	if (position.y < y) {
		position.y = y;
	}
	else {
		position.y = Lerp(position.y, y, 0.1f);
	}
}