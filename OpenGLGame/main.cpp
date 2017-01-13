#include <stdio.h>
#include <stdlib.h>
#include <Engine.h>
#include <Vehicle.h>
#include <ChaseCamera.h>
#include <chrono>
#include <Windows.h>
using namespace glm;

static const float fov = 60.0f;
Camera* mainCamera;
GLuint shaderProgram;

int main(int argc, char* argv) {

	// Check for command line arguments
	if (argc > 0) {
	}

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	Game* game = new Game();

	glfwWindowHint(GLFW_SAMPLES, game->antiAliasing);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

																   // Open a window and create its OpenGL context
	game->window = glfwCreateWindow(game->width, game->height, "Game", game->fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (game->window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(game->window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSwapInterval(1);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(game->window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	Input::SetWindow(game->window);

	shaderProgram = ShaderCompiler::LoadShaders("Vertex.shader", "Fragment.shader");

	// Load the model
	glm::vec3 color = glm::vec3(0, 1, 1);
	Vehicle* mesh = new Vehicle("spaceShip.obj", color, shaderProgram);
	mesh->scale = glm::vec3(0.1f, 0.1f, 0.1f);
	Terrain* terrain = new Terrain(shaderProgram, "heightmap.bmp");
	mesh->terrain = terrain;

	mainCamera = new ChaseCamera(mesh);
	
											   // Get a handle for our "MVP" uniform
											   // Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

	//Reset cursor position
	glfwSetCursorPos(game->window, Game::width / 2, Game::height / 2);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triengles that are not facing the camera to optimize performance
	glEnable(GL_CULL_FACE);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 2.0f);
	float fogColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glFogfv(GL_FOG_COLOR, fogColor);
	
	glm::mat4 Projection = mainCamera->GetProjectionMatrix(fov);

	mesh->Init();

	do {
		Time::Update();

		glClearColor(0, 0, 0, 1); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, game->width, game->height);

		glm::mat4 View = mainCamera->GetViewMatrix();
		// Model matrix
		glm::quat meshRot = quat(mesh->rotation);
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), mesh->position) * glm::scale(glm::mat4(1.0f), mesh->scale) * mat4_cast(meshRot);
		// ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = Projection * View * Model;

		glUseProgram(shaderProgram);

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		mesh->Update();
		mainCamera->Update();

		// Draw the model!
		mesh->Render();
		
		Model = glm::mat4(1.0f);
		mvp = Projection * View * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		terrain->Render();

		glfwSwapBuffers(game->window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(game->window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(game->window) == 0);
}

float Lerp(float x, float y, float a) {
	return (x * (1.0 - a) + y * a);
}

void DrawLine(glm::vec3 p1, glm::vec3 p2) {

	glm::mat4 Projection = mainCamera->GetProjectionMatrix(fov);
	glm::mat4 View = mainCamera->GetViewMatrix();
	glm::mat4 Model = glm::mat4(1.0f);
	// ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model;
	GLuint MatrixID = glGetUniformLocation(GetShaderProgram(), "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	std::vector<glm::vec3> points;
	points.push_back(p1);
	points.push_back(p2);

	GLuint linebuffer;
	glGenBuffers(1, &linebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, linebuffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, linebuffer);
	glVertexAttribPointer(
		0,
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, 2);
}

GLuint GetShaderProgram() {
	return shaderProgram;
}