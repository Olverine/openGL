#pragma once
#include <Engine.h>
class Minimap : public GameObject
{
public:
	Minimap();
	~Minimap();

	void Init();
	void Update();
	void Render();
};

