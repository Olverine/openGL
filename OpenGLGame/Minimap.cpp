#include "Minimap.h"



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

	position = glm::vec3(-0.95, 0, 0);
}


Minimap::~Minimap()
{
}

void Minimap::Update()
{
}

void Minimap::Render()
{
	GameObject::Render();

	SetColor(glm::vec3(1, 0, 0));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	SetColor(glm::vec3(1, 1, 1));
	glDrawArrays(GL_LINE_LOOP, 4, 4);
}
