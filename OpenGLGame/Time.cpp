#include <Engine.h>
#include <chrono>

float lastTime = glfwGetTime();
float deltaTime = 0;

float Time::getDelta() {
	return deltaTime;
}

void Time::Update() {
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;
}