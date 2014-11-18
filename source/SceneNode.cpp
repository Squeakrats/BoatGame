#include "SceneNode.h"
#define GLM_FORCE_RADIANS
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
SceneNode::SceneNode(StrongMeshPtr meshptr): mMatrix(1.0f), mMesh(meshptr), mScale(1.0f) {
}

SceneNode::SceneNode() : mMatrix(1.0f) , mScale(1.0f){
}

void SceneNode::AddChild(StrongSceneNodePtr node) {
	mChildren.push_back(node);
}

void SceneNode::RebuildInnerMatrix(void) {
	//Z Y X? xyz rotations? for now whatever.
	glm::mat4 X(1.0f), Y(1.0f), Z(1.0f), S(1.0f);
	X = glm::rotate(X, mRotation.x, glm::vec3(1, 0, 0));
	Y = glm::rotate(Y, mRotation.y, glm::vec3(0, 1, 0));
	Z = glm::rotate(Z, mRotation.z, glm::vec3(0, 0, 1)); //suoer temp
	S[0][0] = mScale;
	S[1][1] = mScale;
	S[2][2] = mScale;
	mMatrix = Z * Y * X * S;
	mMatrix[3][0] = mPosition.x;
	mMatrix[3][1] = mPosition.y;
	mMatrix[3][2] = mPosition.z;
}
void SceneNode::SetPosition(const glm::vec3& position) {
	mPosition = position;
	mMatrix[3][0] = mPosition.x;
	mMatrix[3][1] = mPosition.y;
	mMatrix[3][2] = mPosition.z;
}

void SceneNode::SetRotation(const glm::vec3& rotation) {
	mRotation = rotation;
	RebuildInnerMatrix();
}
//this should prob go in a rebuildMatrix() function or somthing
void SceneNode::SetScale(float scale) {
	mScale = scale;
	RebuildInnerMatrix();//this needs to be a rebuildMatrix func
}