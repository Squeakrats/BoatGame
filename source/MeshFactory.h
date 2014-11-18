#pragma once
#include "Mesh.h"

class MeshFactory {
public:
	static void dig(std::vector<StrongSubMeshPtr>&, StrongMeshPtr node, aiNode* ainode);
	static StrongMeshPtr ConvertAiScene(std::string);
};