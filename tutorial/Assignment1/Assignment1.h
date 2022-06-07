#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/glfw/renderer.h"

using Eigen::Matrix4f;
using Eigen::Vector4cf;
using Eigen::Vector3cf;

class Assignment1 : public igl::opengl::glfw::Viewer
{
	int active_coeff = 1;
	int iterations = 20;
	float scale = 1.0f;
	Vector4cf coeffs;
	Vector3cf roots;
	int cursor_xpos, cursor_ypos;
	float xpos = 0, ypos = 0;
	void CalcRoots();
	int width, height;
	Renderer* rndr;

public:

	Assignment1();
	void Init(int width, int height);
	void Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const Program> program);
	void KeyPressed(int key);
	void Scroll(int yoffset);
	void UpdateCursorPosition(int x, int y);
	void DragTo(int x, int y);
	void Resize(int width, int height);
};

