#pragma once
#include "igl/opengl/glfw/Viewer.h"

using Eigen::Matrix4f;

using igl::opengl::Program;

class Game : public igl::opengl::glfw::Viewer
{
public:
	void Init();
	void Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const Program> program);
};
