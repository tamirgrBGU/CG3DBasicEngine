#include "Material.h"



Material::Material(unsigned int textureIDs[], unsigned int slots[], unsigned int size)
{
	numOfTexs = size;
	for (int i = 0; i < size; i++)
	{
		texIDs.push_back(textureIDs[i]);
		slotIDs.push_back(slots[i]);
	}
}

unsigned int Material::GetTexture(unsigned int indx)
{
	if (indx < numOfTexs)
		return texIDs[indx];
	return 0;
}
 unsigned int Material::GetSlot(unsigned int indx)
{
	if (indx < numOfTexs)
		return slotIDs[indx];
	return 0;
}

 void Material::Bind(const std::vector<Texture*> textures , int indx)
 {
	 //for (int i = 0; i < numOfTexs; i++)
	// {
		 textures[texIDs[indx]]->Bind(slotIDs[indx]);
		// shaders[shaderIndices[shaderIndx]]->Bind();
	// }
 }

 void Material::rotateTexs()
 {
	 unsigned int tmp = texIDs[0],i=1;
	 for (; i < numOfTexs; i++)
	 {
		 texIDs[i - 1] = texIDs[i]; 
	 }
	 texIDs[i - 1] = tmp;
 }


Material::~Material()
{
	
}


