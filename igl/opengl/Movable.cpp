#include "Movable.h"
#include <iostream>
#include <cmath>

using namespace Eigen;

namespace igl
{

namespace opengl
{

Movable::Movable()
{
	Tout = Affine3d::Identity();
	Tin = Affine3d::Identity();
}

Movable::Movable(const Movable& mov)
{
	Tout = mov.Tout;
	Tin = mov.Tin;
}

Matrix4f Movable::MakeTransScale()
{
	return (Tout.matrix() * Tin.matrix()).cast<float>();
}

Matrix4d Movable::MakeTransScaled()
{
	return (Tout.matrix() * Tin.matrix());
}


Matrix4d Movable::MakeTransd()
{
	Matrix4d mat = Matrix4d::Identity();
	mat.col(3) << Tin.translation(), 1;

	return (Tout.matrix() * mat);
}

void Movable::Translate(Vector3d amt, bool preRotation)
{

	if (preRotation)
		Tout.pretranslate(amt);
	else
		Tin.pretranslate(amt);
}
void Movable::TranslateInSystem(Matrix3d rot, Vector3d amt)
{
	Tout.pretranslate(rot.transpose() * amt);
}

void Movable::SetCenterOfRotation(Vector3d amt)
{
	Tout.pretranslate(amt);
	Tin.pretranslate(-amt);
}

void Movable::Rotate(const Vector3d& rotAxis, double angle, int mode)
{
	if (mode == 1)
		RotateInSystem(rotAxis, angle);
	else
		Rotate(rotAxis, angle);
}


//angle in radians
void Movable::Rotate(Vector3d rotAxis, double angle)
{
	Tout.rotate(AngleAxisd(angle, rotAxis.normalized()));
}

void Movable::RotateInSystem(Vector3d rotAxis, double angle)
{
	Tout.rotate(AngleAxisd(angle, Tout.rotation().transpose() * (rotAxis.normalized())));
}

void Movable::Rotate(const Matrix3d& rot)
{
	Tout.rotate(rot);
}

void Movable::Scale(float factor, Axis axis)
{
	if (abs(factor) < 1e-5)
		return; // factor is too small

	Vector3d factorVec(
		axis == Axis::X || axis == Axis::All ? factor : 1,
		axis == Axis::Y || axis == Axis::All ? factor : 1,
		axis == Axis::Z || axis == Axis::All ? factor : 1
	);

	Tin.scale(factorVec);
}

void Movable::ZeroTrans() {
	Tin = Affine3d::Identity();
	Tout = Affine3d::Identity();
}

} // opengl

} // igl


























//void Movable::TranslateInSystem(Matrix4d Mat, Vector3d amt, bool preRotation)
//{
//	Vector3d v = Mat.transpose().block<3, 3>(0, 0) * amt; //transpose instead of inverse
//	Translate(v, preRotation);
//}
//
//void Movable::RotateInSystem(Matrix4d Mat, Vector3d rotAxis, double angle)
//{
//	Vector3d v = Mat.transpose().block<3, 3>(0, 0) * rotAxis; //transpose instead of inverse
//	Rotate(v.normalized(), angle);
//}
//
//
//void Movable::SetCenterOfRotation(Vector3d amt)
//{
//	Tout.pretranslate(Tout.rotation().matrix().block<3, 3>(0, 0) * amt);
//	Tin.translate(-amt);
//}
//
//Vector3d Movable::GetCenterOfRotation()
//{
//	return Tin.translation();
//}
