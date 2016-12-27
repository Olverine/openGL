#include "Engine.h"



Terrain::Terrain()
{
	sizeX = 32;
	sizeY = 32;
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			glm::vec3 vert = glm::vec3(x, 0, y);
			vertices.push_back(vert);
		}
	}
}


Terrain::~Terrain()
{
}

void Terrain::Render() {
	for (int y = 0; y < sizeY - 1; y++) {
		for (int x = 0; x < sizeX - 1; x++) {
			
		}
	}
}