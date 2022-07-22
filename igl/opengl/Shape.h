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

// private:

	shared_ptr<const Mesh> mesh;
	shared_ptr<const Material> material;
};

} // opengl

} // igl
