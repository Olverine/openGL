#pragma once
#include <Engine.h>
class Road : public GameObject
{
public:
	Road(unsigned char* data, Terrain* terrain);
	~Road();
	void Render();
};