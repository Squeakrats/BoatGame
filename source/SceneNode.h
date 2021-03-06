#pragma once
#define GLM_FORCE_RADIANS
#include "glm/mat4x4.hpp"
#include "Mesh.h"
#include <vector>
#include <memory>

class Renderer;
class SceneNode;

typedef std::shared_ptr<SceneNode> StrongSceneNodePtr;

class SceneNode {
	friend class Renderer;
protected:
	glm::vec3 mPosition;
	glm::vec3 mRotation; //quats?
	float mScale;
	glm::mat4 mMatrix;
	
private:
	StrongMeshPtr mMesh;
	std::vector<StrongSceneNodePtr> mChildren;
public:
	SceneNode();
	SceneNode(StrongMeshPtr);
	void AddChild(StrongSceneNodePtr);
	void SetPosition(const glm::vec3&);
	void SetRotation(const glm::vec3&);
	void SetScale(float);
	void RebuildInnerMatrix(void);
	void SetMesh(StrongMeshPtr mesh) { mMesh = mesh; } //this needs to become private at some point. SceneNodeFactory
};