#include "ChaseCamera.h"

ChaseCamera::ChaseCamera(Mesh* target) {
	this->target = target;
}

ChaseCamera::~ChaseCamera() {
}

void ChaseCamera::Update() {
	position = target->position - /*glm::vec3(0.0f,-15.0f,0.0f)*/(target->GetHorizontalForward() * 120.0f) + glm::vec3(0,14,0);
	horizontalAngle = target->rotation.y - glm::radians(90.0f);
	//verticalAngle = glm::radians(-90.0f);
}