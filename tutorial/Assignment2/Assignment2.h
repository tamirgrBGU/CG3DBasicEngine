#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "./sceneParser.h"

using Eigen::Matrix4f;

class Assignment2 : public igl::opengl::glfw::Viewer
{
	int xResolution, yResolution;
	float xOldPos, yOldPos, xRel, yRel, xPos, yPos;
	bool isPressed;
	bool isRightPressed;
	int sourceIndx;
	float time;

public:

	SceneData sceneData;

	Assignment2();
	void Init();
	void Update(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, shared_ptr<Material> material);
	void SetPosition(int x, int y);
	void WhenRotate();
	void WhenTranslate();
	void Animate() override;
	void ScaleAllShapes(float amt, int viewportIndx);
	void SetPress() { isPressed = !isPressed; }
	void SetRightPress() { isRightPressed = !isRightPressed; }
	float Intersection(Eigen::Vector3f sourcePoint);
	void RotateEye(float amt, bool upDown);
	void LoadSceneDataToShader(Shader* s);
	~Assignment2();
};


