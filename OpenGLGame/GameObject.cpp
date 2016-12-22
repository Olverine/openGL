#include <Engine.h>



GameObject::GameObject() {
	GLfloat v[] = {
		0, 1, 0,
		-1, -1, 0,
		1, -1, 0
	};

	memcpy(verts, v, sizeof(v));
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), verts, GL_STATIC_DRAW);
}


GameObject::~GameObject() {
}
