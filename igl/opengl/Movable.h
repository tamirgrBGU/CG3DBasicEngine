#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>

namespace igl
{

namespace opengl
{

class Movable
{
public:
	enum class Axis { X, Y, Z, All, Reset }; // TODO: separate to 2 enums?

	enum{preRot,postRot,phiRot,thetaRot,psiRot,psiPhiRot};

	Movable();
	Movable(const Movable& mov);
	Eigen::Matrix4f MakeTransScale();
	Eigen::Matrix4d MakeTransd();
	Eigen::Matrix4d MakeTransScaled();
	void Translate(Eigen::Vector3d amt, bool preRotation);
	void TranslateInSystem(Eigen::Matrix3d rot, Eigen::Vector3d amt);
	Eigen::Matrix3d GetRotation() { return Tout.rotation(); }
	void SetCenterOfRotation(Eigen::Vector3d amt);
    void Rotate(const Eigen::Vector3d& rotAxis, double angle, int mode);
    void Rotate(Eigen::Vector3d rotAxis, double angle);
    void RotateInSystem(Eigen::Vector3d rotAxis, double angle);
    void Rotate(const Eigen::Matrix3d &rot);
    void Scale(float factor, Axis axis = Axis::All);

	void ZeroTrans();

	Eigen::Matrix3d GetRotation() const{ return Tout.rotation().matrix(); }
    virtual ~Movable() {}
private:

	Eigen::Affine3d Tout,Tin;
};


} // opengl

} // igl
