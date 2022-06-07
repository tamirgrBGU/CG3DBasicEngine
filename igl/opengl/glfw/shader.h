#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>

using namespace std;

class Shader
{
public:

 /**
	 @brief Create shader object from the GLSL code in the given string
	 @param contents - a string containing the code of the shader
	 @param type     - the shader type
 **/
	Shader(unsigned int type, const string& contents);

	unsigned int GetHandle() const;

	~Shader();

private:

	unsigned int m_handle;

	static void CheckCompileStatus(unsigned int shader);

	// disable copy constructor and assignment operators
	void operator=(const Shader& shader) = delete;
	Shader(const Shader& shader) = delete;
};
