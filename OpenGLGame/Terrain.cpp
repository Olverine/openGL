#include "Engine.h"

GLuint terrainVertexBuffer;
GLuint tShaderProgram;

Terrain::Terrain(GLuint shaderProgram)
{
	tShaderProgram = shaderProgram;
	sizeX = 256;
	sizeY = 256;
	density = .02f;
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			glm::vec3 vert = glm::vec3((x -1) / density, 0, y / density);
			vertices.push_back(vert);
			vert = glm::vec3(x / density, 0, y / density);
			vertices.push_back(vert);
		}
	}
	for (int x = 0; x < sizeX; x++) {
		for (int y = 0; y < sizeY; y++) {
			glm::vec3 vert = glm::vec3(x / density, 0, (y -1) / density);
			vertices.push_back(vert);
			vert = glm::vec3(x / density, 0, y / density);
			vertices.push_back(vert);
		}
	}

	// Generate 1 buffer, put the resulting identifier in terrainVertexBuffer
	glGenBuffers(1, &terrainVertexBuffer);
	// The following commands will talk about our 'terrainVertexBuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}


Terrain::~Terrain()
{
}

void Terrain::Render() {
	GLuint colorID = glGetUniformLocation(tShaderProgram, "inColor"); // Get color id in shader
	glUniform3f(colorID, 1, 0, 1); // Set Color of wireframe

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glVertexAttribPointer(
		0,                  
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	/*for (int y = 0; y < sizeY - 1; y++) {
		for (int x = 0; x < sizeX - 1; x++) {
			glDrawArrays(GL_LINES, (y * sizeY) + x, 2);
		}
	}*/

	glDrawArrays(GL_LINES, 0, vertices.size() * sizeof(glm::vec3));
}