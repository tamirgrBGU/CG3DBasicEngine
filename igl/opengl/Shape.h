#pragma once

#include <iostream>
#include <Eigen/Core>
#include "../material_colors.h"
#include "Mesh.h"
#include "Material.h"
#include "Movable.h"

using namespace std;

namespace igl
{

namespace opengl
{

class Shape : public Movable
{
public:

	Shape(const string& file);
	Shape(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material);

	inline shared_ptr<const Mesh> GetMesh() const { return mesh; }
	inline shared_ptr<const Material> GetMaterial() const { return material; };

	inline void SetShowWireframe(bool show = true) { showWireframe = show; }
	inline bool IsShowWireframe() const { return showWireframe; }
	
	inline void SetStatic(bool _static = true) { isStatic = _static; }
	inline bool IsStatic() const { return isStatic; }
	
	inline const Vector4f GetAmbient() { return ambient; }
	inline const Vector4f GetDiffuse() { return diffuse; }
	inline const Vector4f GetSpecular() { return specular; }

 private:

	shared_ptr<const Mesh> mesh;
	shared_ptr<const Material> material;

	Vector4f ambient = Vector4f(GOLD_AMBIENT[0], GOLD_AMBIENT[1], GOLD_AMBIENT[2], 1.0);
	Vector4f diffuse = Vector4f(GOLD_DIFFUSE[0], GOLD_DIFFUSE[1], GOLD_DIFFUSE[2], 1.0);
	Vector4f specular = Vector4f(GOLD_SPECULAR[0], GOLD_SPECULAR[1], GOLD_SPECULAR[2], 1.0);

	bool showWireframe = false;
	bool isStatic = false;
};

} // opengl

} // igl
