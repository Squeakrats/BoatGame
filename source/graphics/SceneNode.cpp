#include "SceneNode.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <iostream>
SceneNode::SceneNode(StrongMeshPtr meshptr): mMatrix(1.0f), mMesh(meshptr) {
}

SceneNode::SceneNode() : mMatrix(1.0f){
}

void SceneNode::AddChild(StrongSceneNodePtr node) {
	mChildren.push_back(node);
}

void SceneNode::SetPosition(const glm::vec3& position) {
	mMatrix[3][0] = position.x;
	mMatrix[3][1] = position.y;
	mMatrix[3][2] = position.z;
}

void SceneNode::SetRotation(const glm::vec3& rotation) {
	float x = mMatrix[3][0], y = mMatrix[3][1], z = mMatrix[3][2];
	//Z Y X? xyz rotations? for now whatever.
	glm::mat4 X(1.0f), Y(1.0f), Z(1.0f);
	X = glm::rotate(X, rotation.x, glm::vec3(1, 0, 0));
	Y = glm::rotate(Y, rotation.y, glm::vec3(0, 1, 0));
	Z = glm::rotate(Z, rotation.z, glm::vec3(0, 0, 1)); //suoer temp
	mMatrix = Z * Y * X;
	mMatrix[3][0] = x;
	mMatrix[3][1] = y;
	mMatrix[3][2] = z;
}