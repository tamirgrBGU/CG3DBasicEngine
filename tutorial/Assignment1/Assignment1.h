
#pragma once
#include "igl/opengl/glfw/Viewer.h"

class Assignment1 : public igl::opengl::glfw::Viewer
{
    float time;
    Eigen::Vector3cf FindRootsOfReduceEquation(Eigen::Vector2cf reduceCoeffs);
    std::complex<float> NewtonCubicRoot(std::complex<float> num);

public:
    int isQuartic;
    Eigen::Vector4cf coeffs;
    float Qcoeffs[5] = { 1,1,1,1,1 };
    std::complex<float> Qroots[4];
    float r, g, b1;
    Eigen::Vector4f move;
    float zoom;
    float x, y;
    int iterationNumber;
    Assignment1();
    //	Assignment1(float angle,float relationWH,float near, float far);
    void Init();
    void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
    void WhenRotate(bool offset, bool xAxis);
    void ZoomInOrOut(bool offset);
    void MoveHorizontal(float xpos);
    void MoveVertical(float ypos);
    void WhenTranslate();
    void Animate() override;
    void ScaleAllShapes(float amt, int viewportIndx);
    void FindQuarticRoots();
    std::complex<float> cuberoot(std::complex<float> z);
    Eigen::Vector3cf FindCubicRoots();
    void changeMode();

    ~Assignment1(void);
};