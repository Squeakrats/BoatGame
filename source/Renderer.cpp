#include "Renderer.h"
#include "GL/glew.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <stack>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
Renderer::Renderer(void) : mMatrix(1.0f) {};

void Renderer::Push() {
	mMatrixStack.push(mMatrix);
}
void Renderer::Pop(void) {
	mMatrix = mMatrixStack.top();
	mMatrixStack.pop();
}

void Renderer::RenderNode(StrongSceneNodePtr node) {
	Push();
	mMatrix *= node->mMatrix;

	if (node->mMesh != nullptr){
		RenderMesh(node->mMesh);
	}
	

	for (auto child : node->mChildren) {
		RenderNode(child);
	}

	Pop();
}

void Renderer::RenderMesh(StrongMeshPtr mesh) {
	Push();
	mMatrix *= mesh->mMatrix;
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mMatrix));

	for (auto submesh : mesh->mMeshes) {
		glBindVertexArray(submesh->mVAO);
		glDrawArrays(GL_TRIANGLES, 0, submesh->mNumVertices);
		
	}

	for (auto child : mesh->mChildren) {
		RenderMesh(child);
	}

	Pop();
}

void Renderer::Render(StrongSceneNodePtr node, int width, int height, unsigned int program) {//tmp send the program
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 perspective = glm::perspective((float)M_PI/2.0f, (float)width / (float) height, 1.0f, 100.0f);
	glm::mat4 camera = glm::translate(glm::mat4(1.0), glm::vec3(0,0, -60));
	glm::mat4 mvMatrix = perspective * camera;
	Push();
	mMatrix = mvMatrix;
	RenderNode(node);
	Pop();
}