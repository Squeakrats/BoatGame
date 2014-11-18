#pragma once
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include <iostream>
#include <memory>
class Renderer;

class Material;
typedef std::shared_ptr<Material> StrongMaterialPtr;
class Material {
public:
	virtual void PreRender(void) = 0;
protected:
	unsigned int mProgram;
};


//temp have the other materials in here as well
// seriously look into uniblocks. never used them but they look super interesting. these all might change. hoooo welll...
class SolidMaterial;
typedef std::shared_ptr<SolidMaterial> StrongSolidMaterialPtr;
class SolidMaterial : public Material {
private:
	glm::vec3 mColor;
public:
	SolidMaterial(unsigned int program, glm::vec3 color) {
		mProgram = program;
		mColor = color;
	}
	virtual void PreRender(void){
		glUseProgram(mProgram);
		glUniform3fv(1, 1, glm::value_ptr(mColor)); //assumed uniform location 0. 
	}
};

class TexturedMaterial;
typedef std::shared_ptr<TexturedMaterial> StrongTexturedMaterialPtr;
class TexturedMaterial : public Material {
	friend class Renderer;//hmmmm
private:
	//possibly start saving the vertex data?. also implment a clone/copy function for these things so I can modify a single instance, 
	int mProgram; //maybe this should be a hint at the type of the material, rather than a specific program
	int mNumTextures;
	unsigned int* mTextures;
public://I can make the cpp files later
	TexturedMaterial(unsigned int program, int numTextures, unsigned int* textureIds){
		mProgram = program;
		
		mNumTextures = numTextures;
		mTextures = new unsigned int[numTextures];

		for(int i = 0; i < numTextures;i++){
			mTextures[i] = textureIds[i];
		}
	}
	virtual void PreRender(void) {//ASUME ONE FOR NOW!!!!!!!!!!
		glUseProgram(mProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextures[0]);
		//do nothing for now?
	}
};