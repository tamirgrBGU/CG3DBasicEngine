#include "Shader.h"
#include "Debug.h"
#include "gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace igl
{

namespace opengl
{

shared_ptr<const Shader> Shader::GetFixedColorFragmentShader()
{
	static const shared_ptr<const Shader> SHADER = move(make_shared<const Shader>(GL_FRAGMENT_SHADER,
		R"(
			#version 330
			uniform vec4 fixedColor;
			out vec4 Color;
			void main()
			{
				Color = fixedColor;
			}
		)"));

	return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayVertexShader()
{
	static const shared_ptr<const Shader> SHADER = move(make_shared<const Shader>(GL_FRAGMENT_SHADER,
		R"(
			#version 330

			in vec3 position;
			in vec3 color;
			out vec3 color_frag;

			uniform mat4 Proj;
			uniform mat4 View;
			uniform mat4 Model;

			void main()
			{
				color_frag = color;
				gl_Position = Proj * View * Model * vec4 (position, 1.0);
			}
		)"));

	return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayFragmentShader()
{
	static const shared_ptr<const Shader> SHADER = move(make_shared<const Shader>(GL_FRAGMENT_SHADER,
		R"(
			#version 330
			in vec3 color_frag;
			out vec4 outColor;

			void main()
			{
				outColor = vec4(color_frag, 1.0);
			}
		)"));

	return SHADER;
}

shared_ptr<const Shader> Shader::GetOverlayPointsFragmentShader()
{
	static const shared_ptr<const Shader> SHADER = move(make_shared<const Shader>(GL_FRAGMENT_SHADER,
		R"(
			#version 330
			in vec3 color_frag;
			out vec4 outColor;
			void main()
			{
				if (length(gl_PointCoord - vec2(0.5)) > 0.5)
					discard;
				outColor = vec4(color_frag, 1.0);
			}
		)"));

	return SHADER;
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

} // opengl

} // igl
