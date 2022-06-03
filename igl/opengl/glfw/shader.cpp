#define GLEW_STATIC
#include "Shader.h"
#include "../Debug.h"
#include "../gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const string& fileName, unsigned int id, bool overlay) : m_id(id)
{
	m_program = glCreateProgram();

	m_shaders[0] = LoadAndCreateShader(fileName + ".vs", GL_VERTEX_SHADER);
	m_shaders[1] = LoadAndCreateShader(fileName + ".glsl", GL_FRAGMENT_SHADER);

	for (unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(m_program, m_shaders[i]);

	if (overlay)
	{
		glBindAttribLocation(m_program, (int)AttributesOverlay::OV_POSITION_VB, "position");
		glBindAttribLocation(m_program, (int)AttributesOverlay::OV_COLOR, "color");
	}
	else
	{
		glBindAttribLocation(m_program, (int)Attributes::POSITION_VB, "position");
		glBindAttribLocation(m_program, (int)Attributes::NORMAL_VB, "normal");
		glBindAttribLocation(m_program, (int)Attributes::KA_VB, "Ka");
		glBindAttribLocation(m_program, (int)Attributes::KD_VB, "Kd");
		glBindAttribLocation(m_program, (int)Attributes::KS_VB, "Ks");
		glBindAttribLocation(m_program, (int)Attributes::TEXCOORD_VB, "texcoord");
	}

	glLinkProgram(m_program);
	glValidateProgram(m_program);

	debug("program object ", m_program, " id ", id, " linked and validated");
}

Shader::~Shader()
{
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(m_program, m_shaders[i]);
		glDeleteShader(m_shaders[i]);
		debug("shader object ", m_shaders[i], " was deleted");
	}

	glDeleteProgram(m_program);
	debug("program object ", m_program, " was deleted");
}

void Shader::Bind() const {
	glUseProgram(m_program);
}

string Shader::LoadShader(const string& fileName)
{
	debug("loading shader file ", fileName);
	ifstream file(fileName);
	if (!file.is_open())
		throw new invalid_argument("error opening shader file " + fileName);

	file.seekg(0, ios::end);
	size_t len = file.tellg();
	file.seekg(0);
	string contents(len + 1, '\0');
	file.read(&contents[0], len);
	return contents;
}

unsigned int Shader::CreateShader(const string& contents, unsigned int type)
{
	GLuint shader = glCreateShader(type);
	const GLchar* p[1] = { contents.c_str() };
	int lengths[1] = { (int)contents.length() };
	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);
	return shader;
}

unsigned int Shader::LoadAndCreateShader(const string& fileName, unsigned int type)
{
	GLuint shader = CreateShader(LoadShader(fileName), type);
	CheckCompileStatus(shader);
	debug("shader object ", shader, " compiled");
	return shader;
}

void Shader::CheckCompileStatus(unsigned int shader)
{
	GLchar info[1024] = { 0 };
	glGetShaderInfoLog(shader, sizeof(info), NULL, info);
	debug(info[0] == 0 ? "no warnings or errors" : string("\n") + info);
}

void Shader::SetUniform1f(const string& name, float v0) {
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform2f(const string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3f(const string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(const string& name, int v0) {
	glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform2i(const string& name, int v0, int v1)
{
	glUniform2i(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3i(const string& name, int v0, int v1, int v2)
{
	glUniform3i(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4i(const string& name, int v0, int v1, int v2, int v3) {
	glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1ui(const string& name, unsigned int v0) {
	glUniform1ui(GetUniformLocation(name), v0);
}

void Shader::SetUniform2ui(const string& name, unsigned int v0, unsigned int v1)
{
	glUniform2ui(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2)
{
	glUniform3ui(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
	glUniform4ui(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform2fv(const string& name, int count, const Vector2f* vectors)
{
	glUniform2fv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniform3fv(const string& name, int count, const Vector3f* vectors)
{
	glUniform3fv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniform4fv(const string& name, int count, const Vector4f* vectors)
{
	glUniform4fv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniform2iv(const string& name, int count, const Vector2i* vectors)
{
	glUniform2iv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniform3iv(const string& name, int count, const Vector3i* vectors)
{
	glUniform3iv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniform4iv(const string& name, int count, const Vector4i* vectors)
{
	glUniform4iv(GetUniformLocation(name), count, vectors[0].data());
}

void Shader::SetUniformMatrix2f(const string& name, const Matrix2f* matrix) {
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Shader::SetUniformMatrix3f(const string& name, const Matrix3f* matrix) {
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Shader::SetUniformMatrix4f(const string& name, const Matrix4f* matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix->data());
}

void Shader::SetUniformMatrix2fv(const string& name, int count, const Matrix2f* matrices)
{
	glUniformMatrix2fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Shader::SetUniformMatrix3fv(const string& name, int count, const Matrix3f* matrices)
{
	glUniformMatrix3fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

void Shader::SetUniformMatrix4fv(const string& name, int count, const Matrix4f* matrices)
{
	glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, matrices[0].data());
}

int Shader::GetUniformLocation(const string& name) {
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		return m_uniformLocationCache[name];

	int location = glGetUniformLocation(m_program, name.c_str());
	if (location == -1)
		cout << "Warning: uniform '" << name << "' doesn't exist!" << endl;
	m_uniformLocationCache[name] = location;
	return location;
}
