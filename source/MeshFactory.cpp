#include "MeshFactory.h"


void MeshFactory::dig(std::vector<StrongSubMeshPtr>& submeshes, StrongMeshPtr node, aiNode* ainode) {

	for (int i = 0; i < ainode->mNumMeshes; i++){
		node->mMeshes.push_back(submeshes[ainode->mMeshes[i]]);
	}

	for (int i = 0; i < ainode->mNumChildren; i++){
		auto aichild = ainode->mChildren[i];

		StrongMeshPtr childNode(new Mesh());
		aichild->mTransformation = aichild->mTransformation.Transpose();
		//std::cout << aichild->mTransformation[0][0] << " " << aichild->mTransformation[1][0] << " " << aichild->mTransformation[2][0] << " " << aichild->mTransformation[3][0] << std::endl;
		//std::cout << aichild->mTransformation[0][1] << " " << aichild->mTransformation[1][1] << " " << aichild->mTransformation[2][1] << " " << aichild->mTransformation[3][1] << std::endl;
		//std::cout << aichild->mTransformation[0][2] << " " << aichild->mTransformation[1][2] << " " << aichild->mTransformation[2][2] << " " << aichild->mTransformation[3][2] << std::endl;
		//std::cout << aichild->mTransformation[0][3] << " " << aichild->mTransformation[1][3] << " " << aichild->mTransformation[2][3] << " " << aichild->mTransformation[3][3] << std::endl;
		//std::cout << "*******" << std::endl;
		childNode->mMatrix = glm::make_mat4((float*) aichild->mTransformation[0]);
		node->mChildren.push_back(childNode);
		dig(submeshes, childNode, aichild);
	}
}

StrongMeshPtr  MeshFactory::ConvertAiScene(const aiScene* aiscene) {

	if (aiscene == nullptr){
		std::cout << "null aiscene" << std::endl;
		while (1);
	}
	std::vector<StrongSubMeshPtr> meshes;

	for (unsigned int i = 0; i < aiscene->mNumMeshes; i++) {

		aiMesh* aimesh = aiscene->mMeshes[i];
		if (!aimesh->HasPositions() || !aimesh->HasNormals()){
			std::cout << "invalid Mesh" << std::endl;
			while (1);
		}

		float* vertexArray = new float[aimesh->mNumVertices * 3];
		float* normalArray = new float[aimesh->mNumVertices * 3];

		for (unsigned int v = 0; v < aimesh->mNumVertices; v++){
			unsigned int index = v * 3;
			auto vertex = aimesh->mVertices[v];
			auto normal = aimesh->mNormals[v];
			vertexArray[index + 0] = vertex.x;
			vertexArray[index + 1] = vertex.y;
			vertexArray[index + 2] = vertex.z;
			normalArray[index + 0] = normal.x;
			normalArray[index + 1] = normal.y;
			normalArray[index + 2] = normal.z;
		}

		StrongSubMeshPtr mesh(new SubMesh(2));
		mesh->mNumVertices = aimesh->mNumVertices;

		glBindVertexArray(mesh->mVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, vertexArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, normalArray, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		delete vertexArray;
		delete normalArray;
		meshes.push_back(mesh);
	}

	StrongMeshPtr node(new Mesh());
	auto ainode = aiscene->mRootNode;
	dig(meshes, node, ainode);

	return node;

}