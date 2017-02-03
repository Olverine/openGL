#include "Minimap.h"
#include "Vehicle.h"

Minimap::Minimap()
{
	vertices.push_back(glm::vec3(0, 0.2f, 0));
	vertices.push_back(glm::vec3(0, -0.2f, 0));
	vertices.push_back(glm::vec3(0.4f / GetAspectRatio(), 0.2f, 0));
	vertices.push_back(glm::vec3(0.4f / GetAspectRatio(), -0.2f, 0));
	vertices.push_back(glm::vec3(0, 0.2f, 0));
	vertices.push_back(glm::vec3(0, -0.2f, 0));
	vertices.push_back(glm::vec3(0.4f / GetAspectRatio(), -0.2f, 0));
	vertices.push_back(glm::vec3(0.4f / GetAspectRatio(), 0.2f, 0));

	for (int i = 0; i < GetPlayerCount(); i++) {
		vertices.push_back(glm::vec3());
	}

	position = glm::vec3(-0.95, 0, 0);
}


Minimap::~Minimap()
{
}

void Minimap::Init()
{
	GameObject::Init();
}

void Minimap::Update()
{
}

void Minimap::Render()
{
	GameObject::Render();

	SetColor(glm::vec3(0, 0, 0));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	SetColor(glm::vec3(1, 1, 1));
	glDrawArrays(GL_LINE_LOOP, 4, 4);

	for (int i = 0; i < GetPlayerCount(); i++) {
		glm::vec3 playerPosition = GetPlayer(1-i)->position;
		playerPosition /= 127000;
		playerPosition *= 0.4f;
		playerPosition += glm::vec3(0.2f, 0,0);
		playerPosition.x /= GetAspectRatio();
		playerPosition.y = playerPosition.z;
		playerPosition.z = 0;
		vertices[8 + i] = playerPosition;
		Init();
		glPointSize(4);
		SetColor(dynamic_cast<Vehicle*>(GetPlayer(i))->color);
		glDrawArrays(GL_POINTS, 8 + i, 1);
	}
}
