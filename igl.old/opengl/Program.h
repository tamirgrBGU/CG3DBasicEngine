#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>
#include "Shader.h"

using namespace std;
using namespace Eigen;

namespace igl
{

namespace opengl
{

class Program
{
public:

 /**
	 @brief Create a program object with a vertex and fragement shader from 2 files
	 @param fileName - name of the files without extension (extension .vs and .glsl is appended)
	 @param id       - id of the shader (must be 0 to ???) // TODO: TAL
	 @param overlay  - true for full overlay data, false for partial data
 **/
	Program(const string& fileName, unsigned int id, bool overlay = false, bool warnings = true);

/**
	 @brief Create a program object from the given vertex and fragement shader objects
	 @param vertexShader	- a vertex shader object
	 @param fragmentShader	- a fragment shader object
	 @param id				- id of the shader (must be 0 to ???) // TODO: TAL
	 @param overlay			- true for full overlay data, false for partial data
 **/
	Program(shared_ptr<const Shader> vertexShader, shared_ptr<const Shader> fragmentShader, 
		unsigned int id, bool overlay, bool warnings);

 /**
	 @brief Bind the program object
 **/
	void Bind() const;

	inline unsigned int GetHandle() const { return handle; }
	inline unsigned int GetId() const { return id; }
	inline shared_ptr<const Shader> GetVertexShader() const { return vertexShader; }

	~Program();

///@{
 /**
	Mapping methods between Eigen and OpenGL shader language uniforms
	See @ref https://docs.gl/gl4/glUniform
 **/
	void SetUniform1f(const string& name, float v0) const;
	void SetUniform2f(const string& name, float v0, float v1) const;
	void SetUniform3f(const string& name, float v0, float v1, float v2) const;
	void SetUniform4f(const string& name, float v0, float v1, float v2, float v3) const;
	void SetUniform1i(const string& name, int v0) const;
	void SetUniform2i(const string& name, int v0, int v1) const;
	void SetUniform3i(const string& name, int v0, int v1, int v2) const;
	void SetUniform4i(const string& name, int v0, int v1, int v2, int v3) const;
	void SetUniform1ui(const string& name, unsigned int v0) const;
	void SetUniform2ui(const string& name, unsigned int v0, unsigned int v1) const;
	void SetUniform3ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2) const;
	void SetUniform4ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) const;
	void SetUniform2fv(const string& name, int count, const Vector2f* vectors) const;
	void SetUniform3fv(const string& name, int count, const Vector3f* vectors) const;
	void SetUniform4fv(const string& name, int count, const Vector4f* vectors) const;
	void SetUniform2iv(const string& name, int count, const Vector2i* vectors) const;
	void SetUniform3iv(const string& name, int count, const Vector3i* vectors) const;
	void SetUniform4iv(const string& name, int count, const Vector4i* vectors) const;
	void SetUniformMatrix2f(const string& name, const Matrix2f* matrix) const;
	void SetUniformMatrix3f(const string& name, const Matrix3f* matrix) const;
	void SetUniformMatrix4f(const string& name, const Matrix4f* matrix) const;
	void SetUniformMatrix2fv(const string& name, int count, const Matrix2f* matrices) const;
	void SetUniformMatrix3fv(const string& name, int count, const Matrix3f* matrices) const;
	void SetUniformMatrix4fv(const string& name, int count, const Matrix4f* matrices) const;
///@}

private:

	int GetUniformLocation(const string& name) const;
	static string ReadFile(const string& fileName);

	// disable copy constructor and assignment operator
	void operator=(const Program& shader) = delete;
	Program(const Program& shader) = delete;

	static const shared_ptr<const Shader> FIXED_COLOR_SHADER;

	shared_ptr<const Shader> vertexShader;
	shared_ptr<const Shader> fragmentShader;
	unsigned int handle;
	unsigned int id;
	mutable unordered_map<string, int> uniformLocationCache;
	bool warnings;

	enum class Attributes
	{
		POSITION_VB,
		NORMAL_VB,
		KA_VB,
		KD_VB,
		KS_VB,
		TEXCOORD_VB,
		JOINT_INDEX_VB
	};

	enum class AttributesOverlay
	{
		OV_POSITION_VB,
		OV_COLOR,
	};
};

} // opengl

} // igl
