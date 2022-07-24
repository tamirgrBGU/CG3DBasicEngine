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
	
	inline const RowVector4f GetAmbient() { return ambient; }
	inline const RowVector4f GetDiffuse() { return diffuse; }
	inline const RowVector4f GetSpecular() { return specular; }

 private:

	shared_ptr<const Mesh> mesh;
	shared_ptr<const Material> material;

	// TODO: TAL: handle the colors...
	RowVector4f ambient = RowVector4f(1.0, 1.0, 1.0, 1.0);
	RowVector4f diffuse = RowVector4f(1.0, 1.0, 1.0, 1.0);
	RowVector4f specular = RowVector4f(1.0, 1.0, 1.0, 1.0);

	bool showWireframe = false;
	bool isStatic = false;
};

} // opengl

} // igl
