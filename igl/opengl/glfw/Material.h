#pragma once
#include <vector>
#include "texture.h"
#include "shader.h"

using std::vector;
using std::shared_ptr;
using std::string;

class Material
{
	shared_ptr<Shader> m_shader;
	vector<shared_ptr<Texture>> m_textures;
	vector<int> m_textureSlots;

public:

 /**
	 @brief Create a material with a given shader object
	 @param shader - shared pointer to the shader object

 **/
	explicit Material(shared_ptr<Shader> shader);

 /**
	 @brief Create a shader object and use it to create a material object
	 (use for convenience when the shader object is exclusive to this material)
	 @param shaderFileName - filename of the shader object
	 @param overlay        - overlay of the shader object
	 @param shaderId       - id of the shader object
 **/
	Material(const string& shaderFileName, unsigned int shaderId, bool overlay = false);

 /**
	 @brief Add a given texture object to the material
	 @param slot    - the slot to bind the texture to when binding the material
	 @param texture - shared pointer to the texture object
 **/
	void AddTexture(int slot, shared_ptr<Texture> texture);

 /**
	 @brief Creates a texture object from an existing image file and add it to the material
	 (use for convenience when the texture object is exclusive to this material)
	 @param slot            - the slot to bind the texture to when binding the material
	 @param textureFileName - the name of the image file
	 @param dim             - the dimensions of the texture data
 **/
	void AddTexture(int slot, const string& textureFileName, int dim);

 /**
	 @brief Bind all material objects
	 @retval  - a shared pointer to the shader object
 **/
	shared_ptr<Shader> Bind();

	inline shared_ptr<Shader> GetShader() { return m_shader; }
};

