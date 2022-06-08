#include "Shader.h"
#include "Shader.h"
#include "../Debug.h"
#include "../gl.h"
#include <iostream>
#include <fstream>
#include <sstream>


shared_ptr<const Shader> Shader::GetFixedColorShader()
{
	static const shared_ptr<const Shader> FIXED_COLOR_SHADER = move(make_shared<const Shader>(GL_FRAGMENT_SHADER,
		R"(
			#version 330
			uniform vec4 fixedColor;
			out vec4 Color;
			void main()
			{
				Color = fixedColor;
			}
		)"));

	return FIXED_COLOR_SHADER;
}

Shader::Shader(unsigned int type, const string& contents)
{
	m_handle = glCreateShader(type);
	const GLchar* p[1] = { contents.c_str() };
	int lengths[1] = { (int)contents.length() };
	glShaderSource(m_handle, 1, p, lengths);
	glCompileShader(m_handle);
	CheckCompileStatus(m_handle);
}

unsigned int Shader::GetHandle() const
{
	return m_handle;
}

Shader::~Shader()
{
	glDeleteShader(m_handle);
	debug("shader object ", m_handle, " was deleted");
}

void Shader::CheckCompileStatus(unsigned int shader)
{
	GLchar info[1024] = { 0 };
	glGetShaderInfoLog(shader, sizeof(info), NULL, info);
	debug("shader ", shader, " compile status: ", info[0] == 0 ? "no warnings or errors" : string("\n") + info);
}
