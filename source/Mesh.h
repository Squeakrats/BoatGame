#pragma once
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include "GL/glew.h"
#include <list>
#include <vector>
#include "glm/glm.hpp"
#include <memory>
#include <iostream>


class Mesh;
class SubMesh;
class Renderer;

typedef std::shared_ptr<SubMesh> StrongSubMeshPtr;
typedef std::shared_ptr<Mesh> StrongMeshPtr;



class Mesh {
	friend class Renderer;
	friend class MeshFactory;
private:
	std::vector<StrongSubMeshPtr> mMeshes;
	std::vector<StrongMeshPtr> mChildren;
	glm::mat4 mMatrix;
public:
};


class SubMesh {
	friend class Renderer;
	friend class MeshFactory;
private:
	unsigned int mVAO;
	unsigned int numBuffers;
	unsigned int* mBuffers;
	unsigned int mNumBuffers;
	unsigned int mNumVertices;
public:
	SubMesh(unsigned int numBuffers) {
		mNumBuffers = numBuffers;
		mBuffers = new unsigned int[mNumBuffers];
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(mNumBuffers, mBuffers);
	}

	~SubMesh(){
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(numBuffers, mBuffers);
	}
	//make these private at some point :/
	
};


