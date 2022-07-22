#pragma once
#include <string>

using std::string;

namespace igl
{

namespace opengl
{

class Texture
{
public:

 /**
     @brief Create a texture object from an existing image file
     @param fileName - the name of the image file
     @param dim      - the dimensions of the texture data
 **/
	Texture(const string& fileName, const int dim);

 /**
     @brief Create a texture object from pixel data in memory. 
	 Parameter information at @ref https://docs.gl/gl4/glTexImage2D
     @param internalformat - number of color components in the texture (see link)
     @param width          - width of the image
     @param height         - height of the image
     @param format         - format of the pixel data
     @param type           - data type of the pixel data
     @param data           - a pointer to the image data in memory
 **/
	Texture(int internalformat, int width, int height, unsigned int format, unsigned int type, const void* data);

    void Bind(int slot);
    void Unbind(int slot);

    ~Texture();

private:

     unsigned char* LoadFromFile(const string& filename, int* width, int* height, int* numComponents, int reqComponents);
     Texture(int dim); // private constructor to initialize m_type according to dim

     unsigned int m_texture = 0;
     int m_dim;
     int m_type = 0;
};

} // opengl

} // igl
