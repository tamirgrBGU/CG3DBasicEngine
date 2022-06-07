#pragma once
#include "igl/opengl/glfw/Viewer.h"

using Eigen::Matrix4f;

class Game : public igl::opengl::glfw::Viewer
{
private:
	int time;
	void ScaleAllShapes(float amt, int viewportIndx);

public:
	Game();
	//	Game(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<const Program> program) override;
	void Animate() override;
	unsigned int CreateTex(int width, int height);
	~Game(void);
};

