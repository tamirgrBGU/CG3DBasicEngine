#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "./sceneParser.h"
class Assignment2 : public igl::opengl::glfw::Viewer
{
	int xResolution, yResolution;
	float xOldPos, yOldPos, xRel, yRel, xPos, yPos;
	bool isPressed;
	bool isRightPressed;
	int sourceIndx;
public:

	float time;
	Assignment2();
	SceneData scnData;
//	Assignment2(float angle,float relationWH,float near, float far);
	
	void Init();
	void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
	void SetPosition(int x, int y);
	void WhenRotate();
	void WhenTranslate();
	void Animate() override;
	void ScaleAllShapes(float amt, int viewportIndx);
	void SetPress() { isPressed = !isPressed; }
	void SetRightPress() { isRightPressed = !isRightPressed; }
	float Intersection(Eigen::Vector3f sourcePoint);
	void RotateEye(float amt, bool upDown);
	~Assignment2(void);
};


