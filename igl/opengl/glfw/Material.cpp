#include "Material.h"

using std::make_shared;
using std::move;

Material::Material(shared_ptr<const Program> program, bool overlay)
{
	// create another program for picking using the given program's vertex shader and the
	// hard-coded fixed color shader
	m_pickingProgram = move(make_shared<Program>(move(program->GetVertexShader()), 
		move(Program::GetFixedColorShader()), program->GetId(), overlay));
	// TODO: TAL: check if i need a different id for the picking shader program

	m_program = move(program);
}

Material::Material(const string& shaderFileName, unsigned int programId, bool overlay) :
	Material(move(make_shared<Program>(shaderFileName, programId, overlay))) {}

void Material::AddTexture(int slot, shared_ptr<Texture> texture)
{
	m_textures.push_back(move(texture));
	m_textureSlots.push_back(slot);
}

void Material::AddTexture(int slot, const string& textureFileName, int dim)
{
	AddTexture(slot, move(make_shared<Texture>(textureFileName, dim)));
}

shared_ptr<const Program> Material::BindProgram() const
{
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->Bind(m_textureSlots[i]);

	m_program->Bind();

	return m_program;
}

shared_ptr<const Program> Material::BindPicking() const
{
	m_pickingProgram->Bind();

	return m_pickingProgram;
}
