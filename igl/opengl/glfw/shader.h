#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <unordered_map>
#include <Eigen/Core>

class Shader
{
public:
	Shader(const std::string& fileName,unsigned int id);
	Shader(const std::string& fileName,bool overlay,unsigned int id);

    Shader(const std::string &vertex_shader, const std::string &fragment_shader, bool overlay, unsigned int id);

    void Bind() const;
	void Unbind() const;

		//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4i(const std::string& name, int vi0,int vi1,int vi2,int vi3);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3 );
	void SetUniformMat4f(const std::string& name, const Eigen::Matrix4f&  matrix);
	void SetUniformMat4fv(const std::string& name,const Eigen::Matrix4f *matrix,const int length);
	void SetUniform4fv(const std::string& name, const Eigen::Vector4f * arr, const int length);

	 ~Shader();

    unsigned int m_program;
    unsigned int m_id;
protected:
private:
	enum Attributes
	{
		POSITION_VB,
        NORMAL_VB,
        KA_VB,
        KD_VB,
        KS_VB,
		TEXCOORD_VB,
		JOINT_INDEX_VB
	};

    enum Attributes_overlay
    {
        OV_POSITION_VB,
        OV_COLOR,
    };
	static const unsigned int NUM_SHADERS = 2;
	void operator=(const Shader& shader) {}
	Shader(const Shader& shader) {}
	
	std::string LoadShader(const std::string& fileName);
	unsigned int CreateShader(const std::string& text, unsigned int type);

    unsigned int m_shaders[NUM_SHADERS]{};
	std::unordered_map<std::string, int> m_UniformLocationCache;
	int GetUniformLocation (const std::string& name);
	void CheckShaderError(unsigned int shader, unsigned int flag, bool isProgram, const std::string& errorMessage);
};

#endif
