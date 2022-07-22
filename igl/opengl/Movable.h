#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>

using namespace Eigen;

namespace igl
{

namespace opengl
{

class Movable abstract
{
public:
	enum class Axis { X, Y, Z, All, Reset }; // TODO: TAL: make sense... (separate to 2 enums?)

	enum { preRot, postRot, phiRot, thetaRot, psiRot, psiPhiRot };

	Movable();
	Movable(const Movable& mov);
	virtual ~Movable() {};
	Matrix4f MakeTransScale();
	Matrix4d MakeTransd();
	Matrix4d MakeTransScaled();
	void Translate(float dist, Axis axis);
	void Translate(const Vector3d vec);
	void Translate(const Vector3d vec, bool x); // TODO: TAL
	void TranslateInSystem(Matrix3d rot, Vector3d amt);
	Matrix3d GetRotation() { return Tout.rotation(); }
	void SetCenterOfRotation(Vector3d amt);
	void Rotate(double angle, Axis axis);
	void RotateInSystem(double angle, Axis axis);
	void Rotate(const Matrix3d& rot);
	void Scale(float factor, Axis axis = Axis::All);
	void ZeroTrans();
	Matrix3d GetRotation() const { return Tout.rotation().matrix(); }

private:

	static const Vector3d VecAxisX();
	static const Vector3d VecAxisY();
	static const Vector3d VecAxisZ();
	Affine3d Tout, Tin;
};

} // opengl

} // igl
