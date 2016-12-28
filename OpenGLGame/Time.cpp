#include <Engine.h>
#include <chrono>

float lastTime = glfwGetTime();
int nbFrames = 0;
static float deltaTime;

float Time::getDelta() {
	return deltaTime;
}

void Time::Update() {
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
										 // printf and reset timer
		printf("%f ms/frame\n", float(currentTime - lastTime));
		printf("%f fps \n", double(nbFrames));
		nbFrames = 0;
		lastTime += 1.0;
	}
}