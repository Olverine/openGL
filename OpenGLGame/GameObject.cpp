#include <Engine.h>

GameObject::GameObject()
{
	colorID = glGetUniformLocation(GetShaderProgram(), "inColor"); // Get color id in shader
}

GameObject::~GameObject()
{
}

void GameObject::Init() {
	glGenBuffers(1, &vertexbuffer);
	RecalculateVertices();
}
void GameObject::Update() {

}
void GameObject::Render() {
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
}

void GameObject::RecalculateVertices()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
}

void GameObject::SetColor(glm::vec3 color) {
	glUniform3f(colorID, color.r, color.b, color.g);
}