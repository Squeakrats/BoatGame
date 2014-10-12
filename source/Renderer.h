#pragma once
#include "glm/mat4x4.hpp"
#include <stack>
#include "SceneNode.h"
class Renderer {
private :
	std::stack<glm::mat4> mMatrixStack;
	glm::mat4 mMatrix;
	void Push();
	void Pop();
	void RenderNode(StrongSceneNodePtr);
	void RenderMesh(StrongMeshPtr);
public:
	//tmp rendering of a specific mesh :/ 
	void Render(StrongSceneNodePtr, int, int, unsigned int);
	Renderer();
};