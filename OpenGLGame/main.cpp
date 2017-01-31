#include <stdio.h>
#include <stdlib.h>
#include <Engine.h>
#include <Vehicle.h>
#include <ChaseCamera.h>
#include <Road.h>
#include <chrono>
using namespace glm;

static const float fov = 50.0f;
Camera* camera[2];
GLuint shaderProgram;
GLuint quad_vertexbuffer;

// Shady shader stuff
GLuint texID;
GLuint depthID;
GLuint depthTexture;

static int width = 800;
static int height = 600;
static int antiAliasing = 4;
static bool fullscreen = false;
GLFWwindow* window;

unsigned char* levelData; 
unsigned char* LoadMap(const char* file);

int playerCount = 2;

void RenderFrameBuffer(GLuint name, GLuint shaderProgram, GLuint texture);

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

	glfwWindowHint(GLFW_SAMPLES, antiAliasing);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Game", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		system("pause");
		return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		system("pause");
		return -1;
	}

	glfwSwapInterval(1);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	shaderProgram = ShaderCompiler::LoadShaders("Vertex.shader", "Fragment.shader");
	GLuint quad_programID = ShaderCompiler::LoadShaders("Passthrough.vertexshader", "Fog.fragmentshader");

	// Load the model
	glm::vec3 color = glm::vec3(1, 0, 1);
	Vehicle* player[] = { 
		new Vehicle("spaceShip2.obj", color, shaderProgram, GLFW_JOYSTICK_1),
		new Vehicle("spaceShip2.obj", glm::vec3(1, 1, 0), shaderProgram,GLFW_JOYSTICK_2)
	};
	// Load level from file and generate terrain
	levelData = LoadMap("heightmap.bmp");
	Terrain* terrain = new Terrain(shaderProgram, levelData);
	Road* road = new Road(levelData, terrain);
	for (int i = 0; i < playerCount; i++) {
		player[i]->scale = glm::vec3(2, 2, 2);
		player[i]->terrain = terrain;
		camera[i] = new ChaseCamera(player[i]);
	}

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

	//Reset cursor position
	glfwSetCursorPos(window, width / 2, height / 2);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triengles that are not facing the camera to optimize performance
	glEnable(GL_CULL_FACE);

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 2.0f);
	float fogColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glFogfv(GL_FOG_COLOR, fogColor);

	glm::mat4 Projection[] = {
		camera[0]->GetProjectionMatrix(fov),
		camera[1]->GetProjectionMatrix(fov)
	};

	player[0]->Init();
	player[1]->Init();

	// ---------------------------------------------
	// Frame buffer code
	// ---------------------------------------------

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	printf("Generating framebuffers\n");
	GLuint FramebufferName[2] = { 0,0 };
	glGenFramebuffers(2, FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[0]);

	// The texture we're going to render to
	GLuint renderedTexture[2];
	glGenTextures(2, renderedTexture);

	//if (true) {
		// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture[0]);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	//// Alternative : Depth texture. Slower, but you can sample it later in your shader
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture[0], 0);
	//}

	//// Depth texture alternative : 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "2" is the size of DrawBuffers

								   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


	// The fullscreen quad's FBO
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	texID = glGetUniformLocation(quad_programID, "renderedTexture");
	depthID = glGetUniformLocation(quad_programID, "depthTexture");

	do {
		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[0]);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		//	RENDER FOR ALL PLAYERS
		//
		glUseProgram(shaderProgram);
		for (int i = 0; i < playerCount; i++) {
			if (i == 0)
				glViewport(0, 0, width, height / 2);
			else if (i == 1)
				glViewport(0, height / 2, width, height / 2);

			player[i]->Update();
			camera[i]->Update();

			glm::mat4 View = camera[i]->GetViewMatrix();
			glm::mat4 Model; 
			glm::mat4 mvp;

			// Draw all players
			for (int p = 0; p < playerCount; p++) {
				// Model matrix
				glm::quat meshRot = quat(player[p]->rotation);
				glm::mat4 Model = glm::translate(glm::mat4(1.0f), player[p]->position) * glm::scale(glm::mat4(1.0f), player[p]->scale) * mat4_cast(meshRot);
				// ModelViewProjection : multiplication of our 3 matrices
				mvp = Projection[i] * View * Model;

				// Send our transformation to the currently bound shader, in the "MVP" uniform
				// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

				// Draw the model!
				player[p]->Render();
			}

			Model = glm::mat4(1.0f);
			mvp = Projection[0] * View * Model;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
			terrain->Render();
			road->Render();
		}

		//RenderFrameBuffer(FramebufferName[1], quad_programID, renderedTexture[0]);
	// Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0, 0, width, height);
		RenderFrameBuffer(0, quad_programID, renderedTexture[0]);
		//glViewport(0, height / 2, width, height / 2);
		//RenderFrameBuffer(0, quad_programID, renderedTexture[0]);

		// Toggle fullscreen
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			fullscreen = !fullscreen;
			glfwSetWindowMonitor(window, fullscreen ? glfwGetPrimaryMonitor() : NULL, 100, 100, width, height, 60);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}

void RenderFrameBuffer(GLuint name, GLuint shaderProgram, GLuint texture) {
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, name);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(shaderProgram);

	// Bind texture to Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Bind depthTexture to Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(texID, 0);
	glUniform1i(depthID, 1);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

float Lerp(float x, float y, float a) {
	return (x * (1.0 - a) + y * a);
}

/*void DrawLine(glm::vec3 p1, glm::vec3 p2) {

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
}*/

unsigned char* LoadMap(const char* file) {
	printf("Loading level from file %s\n", file);
	FILE* f = fopen(file, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte head

	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);
	return data;
}

GLuint GetShaderProgram() {
	return shaderProgram;
}

int GetWidth() {
	return width;
}

int GetHeight() {
	return height;
}

GLFWwindow* GetWindow() {
	return window;
}