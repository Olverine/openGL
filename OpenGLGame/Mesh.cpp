#include "Engine.h"
#include <vector>

using namespace glm;

Mesh::Mesh(const char * path, glm::vec3 color, GLuint shaderProgram, bool opaque) {
	this->opaque = opaque;

	printf("Generating mesh from file: %s \n", path);

	colorID = glGetUniformLocation(shaderProgram, "inColor"); // Get color id in shader

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;

	this->color = color;
	this->shaderProgram = shaderProgram;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("Failed to read file\n");
				return;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		vertices.push_back(vertex);
	}

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

Mesh::~Mesh() {
}

void Mesh::RenderWireFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set PolygonMode to draw wireframes
	glUniform3f(colorID, color.x, color.y, color.z); // Set Color of wireframe
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset Polygon mode to fill
}

void Mesh::Render() {
	GameObject::Render();

	glUniform3f(colorID, 0, 0, 0); // Reset color to black

	if(opaque)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Render black faces to give the model opacity
	RenderWireFrame();

	glDisableVertexAttribArray(0);
}

vec3 Mesh::GetHorizontalForward() {
	vec3 forward;
	forward.x = cos(rotation.x) * -cos(rotation.y);
	forward.z = sin(rotation.y);
	return forward;
}