#include <stdio.h>
#include <stdlib.h>
#include <Engine.h>
#include <Input.h>
#include <chrono>
using namespace glm;

static const float fov = 70.0f;

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

																   // Open a window and create its OpenGL context
	game->window = glfwCreateWindow(game->width, game->height, "Tutorial 01", game->fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (game->window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(game->window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(game->window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	GLuint programID = ShaderCompiler::LoadShaders("Vertex.shader", "Fragment.shader");

	// Load the model
	glm::vec3 color = glm::vec3(0, 1, 1);
	Model* model = new Model("spaceShip.obj", color, programID, true);
	Terrain* terrain = new Terrain(programID);
	
											   // Get a handle for our "MVP" uniform
											   // Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//Reset cursor position
	glfwSetCursorPos(game->window, Game::width / 2, Game::height / 2);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triengles that are not facing the camera to optimize performance
	glEnable(GL_CULL_FACE);
	
	glm::mat4 Projection = Input::GetProjectionMatrix(fov);

	do {
		Time::Update();

		glClearColor(0, 0, 0, 1); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, game->width, game->height);

		Input::ComputeMatricesFromInput(game->window);
		glm::mat4 View = Input::GetViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

		glUseProgram(programID);

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Draw the model!
		model->Render(programID);
		terrain->Render();

		glfwSwapBuffers(game->window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(game->window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(game->window) == 0);
}