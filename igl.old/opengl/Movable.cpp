#include "Movable.h"
#include <iostream>
#include <cmath>

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

void Movable::TranslateInSystem(Matrix3d rot, Vector3d amt)
{
	Tout.pretranslate(rot.transpose() * amt);
}

void Movable::SetCenterOfRotation(Vector3d amt)
{
	Tout.pretranslate(amt);
	Tin.pretranslate(-amt);
}

void Movable::Rotate(double angle, Axis axis)
{
	const Vector3d vec = axis == Axis::X ? VecAxisX() : axis == Axis::Y ? VecAxisY() : VecAxisZ();
	Tout.rotate(AngleAxisd(angle, vec.normalized()));
}

void Movable::RotateInSystem(double angle, Axis axis)
{
	const Vector3d vec = axis == Axis::X ? VecAxisX() : axis == Axis::Y ? VecAxisY() : VecAxisZ();
	Tout.rotate(AngleAxisd(angle, Tout.rotation().transpose() * (vec.normalized())));
}

void Movable::Rotate(const Matrix3d& rot)
{
	Tout.rotate(rot);
}

void Movable::Translate(const Vector3d vec, bool x)
{
	// TODO: TAL
}

void Movable::Translate(const Vector3d vec)
{
	Tin.pretranslate(vec);
}

void Movable::Translate(float dist, Axis axis)
{
	Vector3d vec(
		axis == Axis::X || axis == Axis::All ? dist : 0.0,
		axis == Axis::Y || axis == Axis::All ? dist : 0.0,
		axis == Axis::Z || axis == Axis::All ? dist : 0.0
	);

	Translate(vec);

	//if (preRotation)
	//	Tout.pretranslate(amt);
	//else
	//	Tin.pretranslate(amt);
}

void Movable::Scale(float factor, Axis axis)
{
	//if (abs(factor) < 1e-5)
	//	return; // factor is too small

	Vector3d vec(
		axis == Axis::X || axis == Axis::All ? factor : 1.0,
		axis == Axis::Y || axis == Axis::All ? factor : 1.0,
		axis == Axis::Z || axis == Axis::All ? factor : 1.0
	);

	Tin.scale(vec);
}

void Movable::ZeroTrans() {
	Tin = Affine3d::Identity();
	Tout = Affine3d::Identity();
}

const Vector3d Movable::VecAxisX()
{
	const static Vector3d vec = { 0.0, 1.0, 0.0 };
	return vec;
}

const Vector3d Movable::VecAxisY()
{
	const static Vector3d vec = { 1.0, 0.0, 0.0 };
	return vec;
}

const Vector3d Movable::VecAxisZ()
{
	const static Vector3d vec = { 0.0, 0.0, 1.0 };
	return vec;
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
