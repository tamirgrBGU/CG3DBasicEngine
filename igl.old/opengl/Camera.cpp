//
// Created by hedi1 on 24/01/2022.
//

#include "Camera.h"
#include "../frustum.h"
#include "../ortho.h"
#include "../PI.h"
#include <iostream>




IGL_INLINE igl::opengl::Camera::Camera(float fov,float relationWH, float zNear, float zFar)
{
    this->_length = 2.0f;
    if(fov > 0) //prerspective
    {
        float fH = tan(fov / 360.0 * igl::PI) * zNear;
        float fW = fH * relationWH;
        frustum(-fW, fW, -fH, fH, zNear, zFar,_projection);
        _ortho = false;
    }
    else  //ortho
    {
        float camera_view_angle = 45.0;
        float h = tan(camera_view_angle/360.0 * igl::PI) * (_length);
        ortho(-h*relationWH, h*relationWH, -h, h, zNear, zFar,_projection);
        _ortho = true;
    }
    this->_near = zNear;
    this->_far = zFar;
    this->_fov = fov;
    this->_relationWH = relationWH;
}

IGL_INLINE igl::opengl::Camera::Camera():Camera(45.0f,1.0f, 1.0f, 100.0f)
{
}

IGL_INLINE void igl::opengl::Camera::SetProjection(float fov, float relationWH)
{
    if( fov>0)
    {
        float fH = tan(fov / 360.0 * igl::PI) * _near;
        float fW = fH * relationWH;
        frustum(-fW, fW, -fH, fH, _near, _far,_projection);
        this->_fov = fov;
        _ortho = false;
    }
    else{
        float camera_view_angle = 45.0;
        float h = tan(camera_view_angle/360.0 * igl::PI) * (_length);
        ortho(-h*relationWH, h*relationWH, -h, h, _near, _far,_projection);
        _ortho = true;
    }
    this->_relationWH = relationWH;
}

IGL_INLINE float igl::opengl::Camera::CalcMoveCoeff(float depth,int width) const
{
    float z = _far + depth * (_near - _far);
    if (_fov > 0)
        return width / _far * z * _near / 2.0f / tanf(_fov / 360 * igl::PI); // / camera z translate;
    else
        return 0.5f*width;
}

