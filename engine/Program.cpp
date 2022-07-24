#include "Program.h"
#include "Debug.h"
#include "gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace igl
{

namespace opengl
{

Program::Program(const string& fileName, unsigned int id, bool overlay, bool warnings) :
	Program(move(make_shared<Shader>(GL_VERTEX_SHADER, ReadFile(fileName + ".vs"))),
		move(make_shared<Shader>(GL_FRAGMENT_SHADER, ReadFile(fileName + ".glsl"))), id, overlay, warnings)
{}

Program::Program(shared_ptr<const Shader> vertexShader, shared_ptr<const Shader> fragmentShader, 
	unsigned int id, bool overlay, bool warnings) :
	id(id), warnings(warnings), vertexShader(vertexShader), fragmentShader(fragmentShader)
{
	handle = glCreateProgram();

	glAttachShader(handle, vertexShader->GetHandle());
	glAttachShader(handle, fragmentShader->GetHandle());

	if (overlay)
	{
		glBindAttribLocation(handle, (int)AttributesOverlay::OV_POSITION_VB, "position");
		glBindAttribLocation(handle, (int)AttributesOverlay::OV_COLOR, "color");
	}
	else
	{
		glBindAttribLocation(handle, (int)Attributes::POSITION_VB, "position");
		glBindAttribLocation(handle, (int)Attributes::NORMAL_VB, "normal");
		glBindAttribLocation(handle, (int)Attributes::KA_VB, "Ka");
		glBindAttribLocation(handle, (int)Attributes::KD_VB, "Kd");
		glBindAttribLocation(handle, (int)Attributes::KS_VB, "Ks");
		glBindAttribLocation(handle, (int)Attributes::TEXCOORD_VB, "texcoord");
	}

	glLinkProgram(handle);
	glValidateProgram(handle);

	debug("program object ", handle, " id ", id, " linked and validated");
}

Program::~Program()
{
	glDetachShader(handle, vertexShader->GetHandle());
	glDetachShader(handle, fragmentShader->GetHandle());
	glDeleteProgram(handle);
	debug("program object ", handle, " was deleted");
}

void Program::Bind() const {
	glUseProgram(handle);
}

void Program::SetUniform1f(const string& name, float v0) const
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Program::SetUniform2f(const string& name, float v0, float v1) const
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3f(const string& name, float v0, float v1, float v2) const
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4f(const string& name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform1i(const string& name, int v0) const
{
	glUniform1i(GetUniformLocation(name), v0);
}

void Program::SetUniform2i(const string& name, int v0, int v1) const
{
	glUniform2i(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3i(const string& name, int v0, int v1, int v2) const
{
	glUniform3i(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4i(const string& name, int v0, int v1, int v2, int v3) const
{
	glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform1ui(const string& name, unsigned int v0) const
{
	glUniform1ui(GetUniformLocation(name), v0);
}

void Program::SetUniform2ui(const string& name, unsigned int v0, unsigned int v1) const
{
	glUniform2ui(GetUniformLocation(name), v0, v1);
}

void Program::SetUniform3ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2) const
{
	glUniform3ui(GetUniformLocation(name), v0, v1, v2);
}

void Program::SetUniform4ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) const
{
	glUniform4ui(GetUniformLocation(name), v0, v1, v2, v3);
}

void Program::SetUniform2fv(const string& name, int count, const Vector2f* vectors) const
{
	glUniform2fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform3fv(const string& name, int count, const Vector3f* vectors) const
{
	glUniform3fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform4fv(const string& name, int count, const Vector4f* vectors) const
{
	glUniform4fv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform2iv(const string& name, int count, const Vector2i* vectors) const
{
	glUniform2iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform3iv(const string& name, int count, const Vector3i* vectors) const
{
	glUniform3iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniform4iv(const string& name, int count, const Vector4i* vectors) const
{
	glUniform4iv(GetUniformLocation(name), count, vectors[0].data());
}

void Program::SetUniformMatrix2f(const string& name, const Matrix2f* matrix) const
{
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix3f(const string& name, const Matrix3f* matrix) const
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix4f(const string& name, const Matrix4f* matrix) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Program::SetUniformMatrix2fv(const string& name, int count, const Matrix2f* matrices) const
{
	glUniformMatrix2fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix3fv(const string& name, int count, const Matrix3f* matrices) const
{
	glUniformMatrix3fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Program::SetUniformMatrix4fv(const string& name, int count, const Matrix4f* matrices) const
{
	glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

string Program::ReadFile(const string& fileName)
{
	debug("loading file ", fileName);
	ifstream file(fileName);
	if (!file.is_open())
		throw new invalid_argument("error opening file " + fileName);

	file.seekg(0, ios::end);
	size_t len = file.tellg();
	file.seekg(0);
	string contents(len + 1, '\0');
	file.read(&contents[0], len);
	return contents;
}

int Program::GetUniformLocation(const string& name) const
{
	if (uniformLocationCache.find(name) != uniformLocationCache.end())
		return uniformLocationCache[name];

	int location = glGetUniformLocation(handle, name.c_str());
	if (location == -1 && warnings)
		cerr << "Warning: ignoring uniform '" << name << "' (doesn't exist in program " << id << ")" << endl;

	uniformLocationCache[name] = location;
	return location;
}

} // opengl

} // igl
