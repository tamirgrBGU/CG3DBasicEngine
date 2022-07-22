#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>

using namespace std;

namespace igl
{

namespace opengl
{

class Shader
{
public:

 /**
	 @brief Create shader object from the GLSL code in the given string
	 @param contents - a string containing the code of the shader
	 @param type     - the shader type
 **/
	Shader(unsigned int type, const string& contents);

	static shared_ptr<const Shader> GetFixedColorFragmentShader();
	static shared_ptr<const Shader> GetOverlayVertexShader();
	static shared_ptr<const Shader> GetOverlayFragmentShader();
	static shared_ptr<const Shader> GetOverlayPointsFragmentShader();

	unsigned int GetHandle() const;

	~Shader();

private:

	unsigned int handle;

	static void CheckCompileStatus(unsigned int shader);

	// disable copy constructor and assignment operators
	void operator=(const Shader& shader) = delete;
	Shader(const Shader& shader) = delete;
};

} // opengl

} // igl
