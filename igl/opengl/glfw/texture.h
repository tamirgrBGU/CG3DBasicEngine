#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
public:
	Texture(const std::string& fileName,const int dim);
	Texture(int width, int height,unsigned char *data);
	Texture(int width, int height);
	void Bind(int slot);

	inline int GetSlot(){return m_texture;}
	 ~Texture();
protected:
private:
	//Texture(const Texture& texture) {}
	void operator=(const Texture& texture) {}
	unsigned int m_texture;
	int texDimention;
};

#endif
