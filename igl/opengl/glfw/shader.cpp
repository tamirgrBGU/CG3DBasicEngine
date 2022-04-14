#define GLEW_STATIC
#include "shader.h"
#include "../gl.h"
#include <iostream>
#include <fstream>

static void printMat(Eigen::Matrix4d phi)
{
	printf(" matrix: \n");
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				printf("%f ", phi(j,i));
			printf("\n");
		}
}

Shader::Shader(const std::string& fileName,unsigned int id): Shader(fileName, false,id)
{
	
}

Shader::Shader(const std::string& fileName,bool overlay,unsigned int id):m_id(id)
{
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(LoadShader(fileName + ".glsl"), GL_FRAGMENT_SHADER);
    for(unsigned int i = 0; i < NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);
    if(overlay){
        glBindAttribLocation(m_program, OV_POSITION_VB, "position");
        glBindAttribLocation(m_program, OV_COLOR, "color");
    }else{
        glBindAttribLocation(m_program, POSITION_VB, "position");
        glBindAttribLocation(m_program, NORMAL_VB, "normal");
        glBindAttribLocation(m_program, KA_VB, "Ka");
        glBindAttribLocation(m_program, KD_VB, "Kd");
        glBindAttribLocation(m_program, KS_VB, "Ks");
        glBindAttribLocation(m_program, TEXCOORD_VB, "texcoord");
    }
    glLinkProgram(m_program);
    glValidateProgram(m_program);

}


Shader::Shader(const std::string& vertex_shader,const std::string& fragment_shader,bool overlay,unsigned int id):m_id(id)
{
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(vertex_shader, GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(fragment_shader, GL_FRAGMENT_SHADER);
    for(unsigned int i = 0; i < NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);
    if(overlay){
        glBindAttribLocation(m_program, OV_POSITION_VB, "position");
        glBindAttribLocation(m_program, OV_COLOR, "color");
    }else{
        glBindAttribLocation(m_program, POSITION_VB, "position");
        glBindAttribLocation(m_program, NORMAL_VB, "normal");
        glBindAttribLocation(m_program, KA_VB, "Ka");
        glBindAttribLocation(m_program, KD_VB, "Kd");
        glBindAttribLocation(m_program, KS_VB, "Ks");
        glBindAttribLocation(m_program, TEXCOORD_VB, "texcoord");
    }
    glLinkProgram(m_program);
    glValidateProgram(m_program);

}


Shader::~Shader()
{
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
    {
        glDetachShader(m_program, m_shaders[i]);
		glDeleteShader(m_shaders[i]);
    }

	glDeleteProgram(m_program);
}

void Shader::Bind() const {
	glUseProgram(m_program);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

std::string Shader::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}



unsigned int Shader::CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader ;
	shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    int lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);
	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}


void Shader::SetUniform1i(const std::string& name, int value) {
	glUniform1i(GetUniformLocation(name),value);
}

void Shader::SetUniform4i(const std::string& name,  int vi0,int vi1,int vi2,int vi3) {
	glUniform4i(GetUniformLocation(name),vi0,vi1,vi2,vi3);
}

void Shader::SetUniform1f(const std::string& name, float value) {
	glUniform1f(GetUniformLocation(name),value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(GetUniformLocation(name),v0,v1,v2,v3);
}

void Shader::SetUniformMat4f(const std::string& name, const Eigen::Matrix4f&  matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.data());
}

void Shader::SetUniformMat4fv(const std::string& name,const Eigen::Matrix4f *matrices,const int length)
{
	glUniformMatrix4fv(GetUniformLocation(name), length, GL_FALSE, matrices[0].data());
}
void Shader::SetUniform4fv(const std::string& name, const Eigen::Vector4f * arr, const int length)
{
	glUniform4fv(GetUniformLocation(name), length, arr[0].data());
}

int Shader::GetUniformLocation(const std::string& name) {
	if (m_UniformLocationCache.find(name)!= m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_program, name.c_str());
	if (location == -1 )
		std::cout<< "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	m_UniformLocationCache[name]= location;
	return location;
}

void Shader::CheckShaderError(unsigned int shader, unsigned int flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}
