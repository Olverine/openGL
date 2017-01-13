#pragma once
#include <Engine.h>

class ChaseCamera : public Camera
{
public:
	ChaseCamera(Mesh* target);
	~ChaseCamera();

	Mesh* target;
	void Update();
};

