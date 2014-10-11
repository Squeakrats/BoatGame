#include "Actor.h"

const glm::vec3& Actor::GetPosition(void) {
	return mPosition;
}

void Actor::SetPosition(glm::vec3& position) {
	mPosition = position;
	Emit(0, nullptr);
}

void Actor::AddPosition(glm::vec3& position) {
	mPosition += position;
	Emit(0, nullptr);
}

const glm::vec3& Actor::GetRotation(void) {
	return mRotation;
}

void Actor::SetRotation(glm::vec3& rotation) {
	mRotation = rotation;
	Emit(0, nullptr);
}