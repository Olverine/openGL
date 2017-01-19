#version 410 core
layout(location = 0) out vec3 color;

uniform vec3 inColor;

void main() {
	color = inColor;
}