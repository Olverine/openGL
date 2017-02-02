#pragma once
#include <Engine.h>
class Fence : public GameObject
{
public:
	Fence(glm::vec2 size);
	~Fence();

	void Render();
	static glm::vec2 GetSize();
};