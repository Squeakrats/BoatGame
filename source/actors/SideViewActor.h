#pragma once
#include "Actor.h"
#include "glm\glm.hpp"
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <memory>
class SideViewActor;

typedef std::tr1::shared_ptr<SideViewActor> StrongSideViewActorPtr;
class SideViewActor : public Actor {
private:

public:
	bool mMovingForward;
	bool mTurningLeft;
	bool mTurningRight;
	//assume that all players are built properly :/
	void MoveLeft(float x) {
		glm::vec3 newPosition = GetPosition();
		newPosition.x -= x;
		SetPosition(newPosition);
	}


	void MoveForward(float k) {
		const glm::vec3 rotation = GetRotation();
		glm::mat4 X(1.0f), Y(1.0f), Z(1.0f);
		X = glm::rotate(X, rotation.x, glm::vec3(1.0, 0.0, 0.0));
		Y = glm::rotate(Y, rotation.y, glm::vec3(0.0, 1.0, 0.0));
		Z = glm::rotate(Z, rotation.z, glm::vec3(0.0, 0.0, 1.0)); //suoer temp
		glm::mat4 transform = Z * Y * X;

		glm::vec4 forward = glm::vec4(0, -k, 0, 0);
		glm::vec4 transformed = transform * forward;
		glm::vec3 translation = glm::vec3(transformed.x, transformed.y, transformed.z);
		AddPosition(translation);

		//get forward vector.
		//add it to position. 
	}


	void RotateY(float t) {
		glm::vec3 newRotation = GetRotation();
		newRotation.y += t;
		SetRotation(newRotation);
	}

	void RotateX(float t) {
		glm::vec3 newRotation = GetRotation();
		newRotation.x += t;
		SetRotation(newRotation);
	}

	void RotateZ(float t) {
		glm::vec3 newRotation = GetRotation();
		newRotation.z += t;
		SetRotation(newRotation);
	}

};