#pragma once
#include "igl/opengl/glfw/Viewer.h"

class Game : public igl::opengl::glfw::Viewer
{
	
public:
	
	Game();
//	Game(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
	void WhenRotate();
	void WhenTranslate();
	void Animate() override;
	void ScaleAllShapes(float amt, int viewportIndx);
	
	~Game(void);
};

