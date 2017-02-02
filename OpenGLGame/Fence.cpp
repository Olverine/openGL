#include "Fence.h"

glm::vec2 fenceSize;

Fence::Fence(glm::vec2 size)
{
	fenceSize = size;

	vertices.push_back(glm::vec3(-size.x / 2, 0, -size.y / 2));
	vertices.push_back(glm::vec3(size.x / 2, 0, -size.y / 2));
	vertices.push_back(glm::vec3(size.x / 2, 0, size.y / 2));
	vertices.push_back(glm::vec3(-size.x / 2, 0, size.y / 2));
}

Fence::~Fence()
{
}

void Fence::Render() {
	GameObject::Render();

	glUniform3f(colorID, 1, 1, 1);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

glm::vec2 Fence::GetSize() {
	return fenceSize;
}