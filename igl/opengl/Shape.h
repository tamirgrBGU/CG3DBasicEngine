#pragma once

#include <iostream>
#include <Eigen/Core>
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

 private:

	shared_ptr<const Mesh> mesh;
	shared_ptr<const Material> material;
	bool showWireframe = false;
	bool isStatic = false;
};

} // opengl

} // igl
