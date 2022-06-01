#include "Material.h"

using std::make_shared;
using std::move;

Material::Material(shared_ptr<Shader> shader) :
	m_shader(move(shader)) {}

Material::Material(const string& shaderFileName, bool overlay, unsigned int shaderId) :
	Material(move(make_shared<Shader>(shaderFileName, overlay, shaderId))) {}

void Material::AddTexture(int slot, shared_ptr<Texture> texture)
{
	m_textures.push_back(move(texture));
	m_textureSlots.push_back(slot);
}

void Material::AddTexture(int slot, const string& textureFileName, int dim)
{
	AddTexture(slot, move(make_shared<Texture>(textureFileName, dim)));
}

shared_ptr<Shader> Material::Bind()
{
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->Bind(m_textureSlots[i]);

	m_shader->Bind();
	return m_shader;
}
