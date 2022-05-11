#pragma once
#include <string>

class Texture
{
public:

	Texture(const std::string& fileName, const int dim);
	Texture(int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data);
	 ~Texture();
	 void Bind(int slot);
	 void Unbind();

private:

	unsigned char* LoadFromFile(const std::string& filename, int* width, int* height, int* numComponents, int reqComponents);
	Texture(int dim); // private constructor to initialize m_type

	unsigned int m_texture = 0;
	int m_dim;
	int m_type = 0;
};
