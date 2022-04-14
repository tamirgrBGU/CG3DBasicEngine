#pragma once
#include <vector>
#include "texture.h"
#include "shader.h"

class Material
{
	//Should use smart pointers for two materials with the same texture.
	unsigned int numOfTexs;
	std::vector<unsigned int> texIDs;
	std::vector<unsigned int> slotIDs;
	std::vector<unsigned int> shaderIndices;
public:
	Material(unsigned int textureIDs[],unsigned int slots[],unsigned int size);
	 unsigned int GetTexture(unsigned int indx);
	 unsigned int GetSlot(unsigned int indx);
	 inline unsigned int GetNumOfTexs() { return numOfTexs; }
	 void Bind(const std::vector<Texture*> textures, int indx);
	 void rotateTexs();


	~Material();
};

