#pragma once
#include <Engine.h>
class Road : public GameObject
{
std::vector<glm::vec3> vertices;
public:
	Road(unsigned char* data, Terrain* terrain);
	~Road();
	void Render();
};