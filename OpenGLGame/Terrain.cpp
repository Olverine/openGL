#include "Engine.h"

GLuint terrainVertexBuffer;
GLuint tShaderProgram;

float terrainHeight[256][256];

Terrain::Terrain(GLuint shaderProgram, const char* heightmap)
{
	int i;
	FILE* f = fopen(heightmap, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte head
											   
	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3* width * height;
	printf("Generating %i * %i terrain \n", width, height);
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (i = 0; i < size; i ++)
	{
		//unsigned char tmp = data[i] + data[(i)+1] + data[(i)+2];
	}

	tShaderProgram = shaderProgram;
	sizeX = width;
	sizeY = height;
	density = 0.002f;
	heightScale = 40.0f;
	physicalSize = glm::vec2(sizeX / density, sizeY / density);
	int halfSize = sizeX / density / 2;
	float wireframeOffset = 10;
	// Draw Lines in the x-axis
	for (int y = 0; y < sizeY; y++) {
		for (int x = 1; x < sizeX; x++) {
			glm::vec3 vert = glm::vec3((x-1) / density - halfSize, data[y * height * 3 + ((x-1) * 3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * height * 3 + (x * 3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
			terrainHeight[x][y] = vert.y;
		}
	}
	// Draw lines in the y-axis
	for (int x = 0; x < sizeX; x++) {
		for (int y = 1; y < sizeY; y++) {
			glm::vec3 vert = glm::vec3(x / density - halfSize, data[(y-1) * height * 3 + (x * 3)] * heightScale, (y-1) / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * height * 3 + (x*3)] * heightScale, y / density - halfSize);
			vertices.push_back(vert);
		}
	}
	// Draw faces to make the terrain opaque
	for (int y = 1; y < sizeY; y++) {
		for (int x = 1; x < sizeX; x++) {
			// first triangle
			glm::vec3 vert = glm::vec3((x - 1) / density - halfSize, data[y * height * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[y * height * 3 + (x * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[(y-1) * height * 3 + (x * 3)] * heightScale - wireframeOffset, (y-1) / density - halfSize);
			vertices.push_back(vert);

			// second triangle
			vert = glm::vec3((x - 1) / density - halfSize, data[y * height * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, y / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3(x / density - halfSize, data[(y - 1) * height * 3 + (x * 3)] * heightScale - wireframeOffset, (y - 1) / density - halfSize);
			vertices.push_back(vert);
			vert = glm::vec3((x - 1) / density - halfSize, data[(y - 1) * height * 3 + ((x - 1) * 3)] * heightScale - wireframeOffset, (y - 1) / density - halfSize);
			vertices.push_back(vert);
		}
	}

	// Generate 1 buffer, put the resulting identifier in terrainVertexBuffer
	glGenBuffers(1, &terrainVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

Terrain::~Terrain()
{
}

void Terrain::Render() {
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

	GLuint colorID = glGetUniformLocation(tShaderProgram, "inColor"); // Get color id in shader
	// Draw faces first to make the terrain opaque
	glUniform3f(colorID, 0, 0.01, 0.01); // Set Color of the faces
	glDrawArrays(GL_TRIANGLES, sizeX * sizeY * 4 + (sizeX +1), sizeX*sizeY *6);
	// Draw the grid
	glUniform3f(colorID, 0, 1, 1); // Set Color of wireframe
	glDrawArrays(GL_LINES, 0, sizeX * sizeY * 4);
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