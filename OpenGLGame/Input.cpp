#include <Input.h>

Input::Input()
{
}


Input::~Input()
{
}

Input* input = new Input();
glm::vec3 up;
glm::vec3 direction;

void Input::ComputeMatricesFromInput(GLFWwindow* window) {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	// Reset mouse position for next frame
	glfwSetCursorPos(window, Game::width / 2, Game::height / 2);

	int sizeX, sizeY;
	glfwGetWindowSize(window, &sizeX, &sizeY);

	// Compute new orientation
	input->horizontalAngle += input->mouseSpeed * float( sizeX/ 2 - xpos);
	input->verticalAngle += input->mouseSpeed * float(sizeY / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(input->verticalAngle) * sin(input->horizontalAngle),
		sin(input->verticalAngle),
		cos(input->verticalAngle) * cos(input->horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(input->horizontalAngle - 3.14f / 2.0f),
		0,
		cos(input->horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector : perpendicular to both direction and right
	up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		input->position += direction * Time::getDelta() * input->speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		input->position -= direction * Time::getDelta() * input->speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		input->position += right * Time::getDelta() * input->speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		input->position -= right * Time::getDelta() * input->speed;
	}
}

glm::mat4 Input::GetProjectionMatrix(float fov) {
	glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)Game::width / (float)Game::height, 0.1f, 100.0f);
	return Projection;
}

glm::mat4 Input::GetViewMatrix() {
	glm::mat4 View = glm::lookAt(
		input->position, // Position
		input->position + direction, // Look at
		up  // Up vector
	);
	return View;
}