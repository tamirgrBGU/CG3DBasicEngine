#define GLEW_STATIC
#include "Texture.h"
#include "gl.h"
#include "Debug.h"
#include "glfw/stb_image.h"
#include <iostream>
#include <cassert>

using namespace std;

namespace igl
{

namespace opengl
{

Texture::Texture(const int dim) : m_dim(dim)
{
	assert(dim > 0 && dim < 4);

	switch (dim)
	{
	case 1:
		m_type = GL_TEXTURE_1D;
		break;
	case 2:
		m_type = GL_TEXTURE_2D;
		break;
	case 3:
		m_type = GL_TEXTURE_CUBE_MAP;
		break;
	}
}

Texture::Texture(const string& fileName, const int dim) : Texture(dim)
{
	int width, height, numComponents;
	unsigned char* data = NULL;


	glGenTextures(1, &m_texture);
	glBindTexture(m_type, m_texture);
	switch (dim)
	{
	case 1:
		debug("loading ", dim, " dimensions texture file ", fileName);
		data = LoadFromFile(fileName, &width, &height, &numComponents, 4);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;

	case 2: 
		debug("loading ", dim, " dimensions texture file ", fileName);
		data = LoadFromFile(fileName, &width, &height, &numComponents, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;

	case 3: // cube map
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		{
			string directions[] = { "Right","Left","Top","Bottom","Front","Back" };
			for (int i = 0; i < 6; i++)
			{
				string fullFileName = fileName + directions[i] + ".bmp";
				debug("loading cube map texture file ", fullFileName);
				data = LoadFromFile(fullFileName, &width, &height, &numComponents, 4);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
		}
		break;
	}
	glBindTexture(m_type, 0);
	stbi_image_free(data);
}

Texture::Texture(int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data) : Texture(height > 0 ? 2 : 1)
{
	debug("creating ", height > 0 ? 2 : 1, " dimensions texture file of size ", width, "x", height, " pixels");
	glGenTextures(1, &m_texture);
	glBindTexture(m_type, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	if (m_dim == 2) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
	glBindTexture(m_type, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

unsigned char* Texture::LoadFromFile(const string& fileName, int* width, int* height, int* numComponents, int reqComponents)
{
	unsigned char* data = stbi_load(fileName.c_str(), width, height, numComponents, reqComponents);
	if (data == NULL)
		throw new runtime_error(fileName + " stbi_load error");
	return data;
}

void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_type, m_texture);
}

void Texture::Unbind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_type, 0);
}

} // opengl

} // igl
