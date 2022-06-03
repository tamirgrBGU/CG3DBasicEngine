#pragma once

#include <string>
#include <unordered_map>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

class Shader
{
public:

 /**
	 @brief Create shader object with a vertex and fragement shader from 2 files
	 @param fileName - name of the files without extension (extension .vs and .glsl is appended)
	 @param id       - id of the shader (must be 0 to ???) // TODDO: TAL
	 @param overlay  - true for full overlay data, false for partial data
 **/
	Shader(const string& fileName, unsigned int id, bool overlay = false);

 /**
	 @brief Bind the shader object
 **/
	void Bind() const;

	inline unsigned int GetProgram() { return m_program; }
	inline unsigned int GetId() { return m_id; }

	~Shader();

///@{
 /**
	Mapping methods between Eigen and OpenGL shader language uniforms
	See @ref https://docs.gl/gl4/glUniform
 **/
	void SetUniform1f(const string& name, float v0);
	void SetUniform2f(const string& name, float v0, float v1);
	void SetUniform3f(const string& name, float v0, float v1, float v2);
	void SetUniform4f(const string& name, float v0, float v1, float v2, float v3);
	void SetUniform1i(const string& name, int v0);
	void SetUniform2i(const string& name, int v0, int v1);
	void SetUniform3i(const string& name, int v0, int v1, int v2);
	void SetUniform4i(const string& name, int v0, int v1, int v2, int v3);
	void SetUniform1ui(const string& name, unsigned int v0);
	void SetUniform2ui(const string& name, unsigned int v0, unsigned int v1);
	void SetUniform3ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2);
	void SetUniform4ui(const string& name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3);
	void SetUniform2fv(const string& name, int count, const Vector2f* vectors);
	void SetUniform3fv(const string& name, int count, const Vector3f* vectors);
	void SetUniform4fv(const string& name, int count, const Vector4f* vectors);
	void SetUniform2iv(const string& name, int count, const Vector2i* vectors);
	void SetUniform3iv(const string& name, int count, const Vector3i* vectors);
	void SetUniform4iv(const string& name, int count, const Vector4i* vectors);
	void SetUniformMatrix2f(const string& name, const Matrix2f* matrix);
	void SetUniformMatrix3f(const string& name, const Matrix3f* matrix);
	void SetUniformMatrix4f(const string& name, const Matrix4f* matrix);
	void SetUniformMatrix2fv(const string& name, int count, const Matrix2f* matrices);
	void SetUniformMatrix3fv(const string& name, int count, const Matrix3f* matrices);
	void SetUniformMatrix4fv(const string& name, int count, const Matrix4f* matrices);
///@}

private:

	static const int NUM_SHADERS = 2;
	unsigned int m_program;
	unsigned int m_id;
	unsigned int m_shaders[NUM_SHADERS] {};
	unordered_map<string, int> m_uniformLocationCache;

	static unsigned int CreateShader(const string& contents, unsigned int type);
	static string LoadShader(const string& fileName);
	static unsigned int LoadAndCreateShader(const string& fileName, unsigned int type);
	static void CheckCompileStatus(unsigned int shader);
	int GetUniformLocation(const string& name);

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

	// disable copy constructor and assignment operator
	void operator=(const Shader& shader) = delete;
	Shader(const Shader& shader) = delete;
};
