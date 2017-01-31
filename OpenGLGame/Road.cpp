#include "Road.h"
#include <vector>

glm::vec2 nodes[255];
std::vector<glm::vec3> vertices;

// vertex buffer identity
GLuint roadVertexbuffer;
// id of color in shader
GLuint roadColorID;

Road::Road(unsigned char* data, Terrain* terrain)
{
	printf("Generating Road: ");
	int foundNodes = 0;
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < 256; y++) {
			if (data[(y * 256 * 3 + (x * 3))+2] == 255) {
				nodes[(int)data[(y * 256 * 3 + (x * 3)) + 1]-1] = glm::vec2(x, y);
				foundNodes++;
			}
		}
	}
	printf("found %i nodes\n", foundNodes);
	int halfSize = 256 / 0.002f / 2;
	for (int i = 0; i < foundNodes; i++) {
		printf("%f, %f\n", nodes[i].x, nodes[i].y);
		vertices.push_back(glm::vec3(nodes[i].x / 0.002f - halfSize, terrain->GetElevation(nodes[i].x / 0.002f - halfSize, nodes[i].y / 0.002f - halfSize), nodes[i].y / 0.002f - halfSize));
	}

	roadColorID = glGetUniformLocation(GetShaderProgram(), "inColor"); // Get color id in shader

	glGenBuffers(1, &roadVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, roadVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

void Road::Render() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, roadVertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glUniform3f(roadColorID, 1, 0, 0); // Reset color to black

	glDrawArrays(GL_LINE_LOOP, 0, vertices.size());

	glDisableVertexAttribArray(0);
}

Road::~Road()
{
}