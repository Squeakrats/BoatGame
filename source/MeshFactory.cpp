#include "MeshFactory.h"
#include <map>

#include "Material.h"
#include "glm/glm.hpp"
#include "util.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <FreeImage.h>
#include "GL/glew.h"

#include <assimp/Importer.hpp>  
#include <assimp/scene.h>  
#include <assimp/postprocess.h>
#include <deque>

Assimp::Importer importer;

void MeshFactory::dig(std::vector<StrongSubMeshPtr>& submeshes, StrongMeshPtr node, aiNode* ainode) {

	for (int i = 0; i < ainode->mNumMeshes; i++){
		node->mMeshes.push_back(submeshes[ainode->mMeshes[i]]);
	}

	for (int i = 0; i < ainode->mNumChildren; i++){
		auto aichild = ainode->mChildren[i];

		StrongMeshPtr childNode(new Mesh());
		aichild->mTransformation = aichild->mTransformation.Transpose();
		childNode->mMatrix = glm::make_mat4((float*) aichild->mTransformation[0]);
		node->mChildren.push_back(childNode);
		dig(submeshes, childNode, aichild);
	}
}

//this thing needs access to a table of shaders. 
// 0 = solid color
// 1 = textured
// 3 = toonshading?
// 4 = deffered solid color
// 5 = deffered textured
// 6 = deffered toon???? lelwat. 
StrongMeshPtr  MeshFactory::ConvertAiScene(std::string path) {//need some way of defining wich shader to run on a given object. like graphicsProfile. 
	
	const aiScene* aiscene = importer.ReadFile(path.c_str(), aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_Triangulate );

	if (aiscene == nullptr){
		std::cout << "null aiscene" << std::endl;
		while (1);
	}

	ilInit();

	std::string basePath = "./" + path.substr(0, path.find_last_of("/") + 1);

	//std::cout << aiscene->mNumMeshes << std::endl;
	//std::cout << aiscene->mMeshes[0]->mNumVertices << std::endl;
	//std::cout << aiscene->mNumMaterials << std::endl;

	// temp create a single material to just throw on everything
	unsigned int solidProg = createProgram("assets/shaders/solidColorv.glsl", "assets/shaders/solidColorf.glsl");
	unsigned int texProg = createProgram("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");
	StrongSolidMaterialPtr material(new SolidMaterial(solidProg, glm::vec3(1.0, 0.5, 0.5)));
	//StrongTexturedMaterialPtr texturedMaterial(new TexturedMaterial(prog, 1, textureIds));
	
	//build a list of every texture
	std::map<std::string, unsigned int> textureMap;
	for(int i = 0; i < aiscene->mNumMaterials;i++){
		aiString path;
		int index = 0;
		while(aiscene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, index, &path) == AI_SUCCESS){
			index++;
			textureMap[path.data] = 0;
		}
	}

	int numTextures = textureMap.size();
	unsigned int* texIds   = new unsigned int[numTextures];
	glGenTextures(numTextures, texIds);

	int texIndex = 0;
	for(auto itr = textureMap.begin(); itr != textureMap.end();++itr){
		std::string fileName = itr->first;
		std::string filePath = basePath + fileName;
		//FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(filePath.data(), 0), filePath.data());
		//int width = FreeImage_GetWidth(bitmap), height = FreeImage_GetHeight(bitmap);
		//auto data = FreeImage_GetBits(bitmap); //do I need to delete this data???????????

		ILuint ImgId = 0;
		ilGenImages(1, &ImgId);
		ilBindImage(ImgId);
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		if(!ilLoadImage(filePath.c_str())){
			std::cout << filePath << " failed to load" << std::endl;
			switch(ilGetError()){
				case IL_COULD_NOT_OPEN_FILE:
					std::cout << "IL_COULD_NOT_OPEN_FILE"<< std::endl;
				break;
				case IL_ILLEGAL_OPERATION:
					std::cout << "IL_ILLEGAL_OPERATION"<< std::endl;
				break;
				case IL_INVALID_EXTENSION:
					std::cout << "IL_INVALID_EXTENSION"<< std::endl;
				break;
				case IL_INVALID_PARAM:
					std::cout << "IL_INVALID_PARAM"<< std::endl;
				break;
				default:
					std::cout << "unknown error" << std::endl;
				break;

			}
		}
		int width = ilGetInteger(IL_IMAGE_WIDTH);
		int height = ilGetInteger(IL_IMAGE_HEIGHT);
		auto data = new BYTE[width * height * 4];
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);//assumed png for now :/
		ilBindImage(0);
		ilDeleteImage(ImgId);

		glBindTexture(GL_TEXTURE_2D, texIds[texIndex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		itr->second = texIds[texIndex];
		texIndex++;
		//FreeImage_Unload(bitmap);
	}

	//FreeImage_Unload(bitmap);

	//eventually we want a diffuse, specular + normal map and the ability to load models with different combos
	StrongMaterialPtr* materials = new StrongMaterialPtr[aiscene->mNumMaterials];
	for(int i = 0; i < aiscene->mNumMaterials;i++){
		aiString path;
		//FOR NOW ASSUME THERE IS ONLY ONE TEXTURE PER MATERIAL. OKAY ALSO THAT IT IS DIFFUSE. ALSO THAT THEY ARE ALL MATERIAL 
		if(aiscene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS){
			unsigned int diffuseTexture = textureMap[path.data];
			materials[i] = StrongTexturedMaterialPtr(new TexturedMaterial(texProg, 1, &diffuseTexture));//assumed one texture for now ;/
			//load the texture. drop it in. 
		}else{
			//COLOR IT RED!!!!!!!!!! also have option for gourard vs phong??????????????do that later. for now just a solid color kthxbai. 
			aiColor3D diffuse;
			aiscene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			diffuse.r = 1.0;
			diffuse.g = 0.0;
			diffuse.b = 0.0;
			materials[i] = StrongSolidMaterialPtr(new SolidMaterial(solidProg, glm::vec3(diffuse.r, diffuse.g, diffuse.b)));
		}
		
	}

	//while(1);


	std::vector<StrongSubMeshPtr> meshes;

	for (unsigned int i = 0; i < aiscene->mNumMeshes; i++) {


		aiMesh* aimesh = aiscene->mMeshes[i];
		int numBuffers = 1 + (int)aimesh->HasPositions() + (int)aimesh->HasNormals() + (int)aimesh->GetNumUVChannels();
		int bufferSize = sizeof(float) * aimesh->mNumVertices * 3;
		float* bufferData = new float[bufferSize];

		StrongSubMeshPtr mesh(new SubMesh(numBuffers));
		//SUPER TEMP ATTACH GENERIC MATERIAL TO EVERY ITEM
		mesh->mMaterial = materials[aimesh->mMaterialIndex];
		mesh->mNumVertices = aimesh->mNumVertices;
		glBindVertexArray(mesh->mVAO);


		//create index buffer
		int numIndices = aimesh->mNumFaces * 3;
		mesh->mNumIndices = numIndices;
		unsigned int* indexData = new unsigned int[numIndices];
		for(int i = 0; i < aimesh->mNumFaces;i++){
			aiFace& face = aimesh->mFaces[i];
			int index = i * 3;
			indexData[index + 0] = face.mIndices[0];
			indexData[index + 1] = face.mIndices[1];
			indexData[index + 2] = face.mIndices[2];
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mBuffers[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

		if(aimesh->HasPositions()){
			for (int v = 0; v < aimesh->mNumVertices; v++){
				int index = v * 3;
				auto vertex = aimesh->mVertices[v];
				bufferData[index + 0] = vertex.x;
				bufferData[index + 1] = vertex.y;
				bufferData[index + 2] = vertex.z;
			}
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[1]);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if(aimesh->HasNormals()){
			for (int v = 0; v < aimesh->mNumVertices; v++){
				int index = v * 3;
				auto normal = aimesh->mNormals[v];
				bufferData[index + 0] = normal.x;
				bufferData[index + 1] = normal.y;
				bufferData[index + 2] = normal.z;
			}
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[2]);
			glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if(aimesh->GetNumUVChannels()){//assume one for now THIS IS TOTALLY WONKERS. RANDOM UV COORDS FOR NOW. 
			for (int v = 0; v < aimesh->mNumVertices; v++){
				int index = v * 2;
				auto texCoord = aimesh->mTextureCoords[0][v];
				bufferData[index + 0] = texCoord.x;
				bufferData[index + 1] = texCoord.y;
			}

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aimesh->mNumVertices * 2, bufferData, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		}

		delete bufferData;
		glBindBuffer(GL_ARRAY_BUFFER, mesh->mBuffers[0]);
		glBindVertexArray(0);
		meshes.push_back(mesh);
	}

	StrongMeshPtr node(new Mesh());
	auto ainode = aiscene->mRootNode;
	dig(meshes, node, ainode);

	return node;

}