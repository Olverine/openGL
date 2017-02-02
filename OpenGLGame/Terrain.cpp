#include "Engine.h"

GLuint tShaderProgram;

float terrainHeight[256][256];

Terrain::Terrain(GLuint shaderProgram, unsigned char* data)
{
	printf("Generating terrain\n");
	tShaderProgram = shaderProgram;
	sizeX = 256;
	sizeY = 256;
	density = 0.002f;
	heightScale = 4.0f;
	physicalSize = glm::vec2(sizeX / density, sizeY / density);
	int halfSize = sizeX / density / 2;
	float wireframeOffset = 10;
	// Draw Lines in the x-axis
	for (int y = 0; y < sizeY; y++) {
		for (int x = 1; x < sizeX; x++) {
			glm::vec3 vert = glm::vec3((x-1) / density - halfSize, data[y * sizeY * 3 + ((x-1) * 3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * sizeY * 3 + (x * 3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
			terrainHeight[x][y] = vert.y;
		}
	}
	// Draw lines in the y-axis
	for (int x = 0; x < sizeX; x++) {
		for (int y = 1; y < sizeY; y++) {
			glm::vec3 vert = glm::vec3(x / density - halfSize, data[(y-1) * sizeY * 3 + (x * 3)] * heightScale, (y-1) / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * sizeY * 3 + (x*3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
		}
	}
	// Draw faces to make the terrain opaque
	for (int y = 1; y < sizeY; y++) {
		for (int x = 1; x < sizeX; x++) {
			// first triangle
			glm::vec3 vert = glm::vec3((x - 1) / density - halfSize, data[y * sizeY * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * sizeY * 3 + (x * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[(y-1) * sizeY * 3 + (x * 3)] * heightScale - wireframeOffset, (y-1) / density - halfSize);
			vertices.push_back(vert);

			// second triangle
			vert = glm::vec3((x - 1) / density - halfSize, data[y * sizeY * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[(y - 1) * sizeY * 3 + (x * 3)] * heightScale - wireframeOffset, (y - 1) / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3((x - 1) / density - halfSize, data[(y - 1) * sizeY * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, (y - 1) / density - halfSize);
			vertices.push_back(vert);
		}
	}

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

Terrain::~Terrain()
{
}

void Terrain::Render() {
	GameObject::Render();

	// Draw faces first to make the terrain opaque
	glUniform3f(colorID, 0, 0.01, 0.01); // Set Color of the faces
	glDrawArrays(GL_TRIANGLES, (sizeX * sizeY * 4) - 1030, sizeX*sizeY *6);
	// Draw the grid
	SetColor(glm::vec3(0, 1, 1)); // Set Color of wireframe
	glDrawArrays(GL_LINES, 0, (sizeX * sizeY * 4) - 1030);
}

float Terrain::GetElevation(float x, float y) {
	int halfSize = sizeX / density / 2;
	float elevation = 0;
	x += halfSize;
	y += halfSize;
	x *= density;
	y *= density;
	float vertexHeight = Lerp(terrainHeight[(int)x][(int)y], terrainHeight[(int)x + 1][(int)y], x - floor(x));
	float nextVertexHeight = Lerp(terrainHeight[(int)x][(int)y + 1], terrainHeight[(int)x + 1][(int)y + 1], x - floor(x));
	float height = Lerp(vertexHeight, nextVertexHeight, y - floor(y));
	return height + 100;
}

glm::vec3 Terrain::GetNormal(float x, float y) {
	int halfSize = sizeX / density / 2;
	x += halfSize;
	y += halfSize;
	x *= density;
	y *= density; 

	glm::vec3 firstVector;
	glm::vec3 secondVector;
	float height;

	if (x - floor(x) < 0.5f && y - floor(y) < 0.5f) {
		height = terrainHeight[(int)x][(int)y];
		firstVector = glm::vec3(1 / density, terrainHeight[(int)x + 1][(int)y] - height, 0);
		secondVector = glm::vec3(0, terrainHeight[(int)x][(int)y + 1] - height, 1 / density);
	}
	else {
		height = terrainHeight[(int)x + 1][(int)y + 1];
		firstVector = glm::vec3(1 / density, terrainHeight[(int)x][(int)y + 1] - height, 0);
		secondVector = glm::vec3(0, terrainHeight[(int)x + 1][(int)y] - height, 1 / density);
	}
	glm::vec3 normal = glm::cross(firstVector, secondVector);
	return glm::normalize(normal) * glm::vec3(-1);
}