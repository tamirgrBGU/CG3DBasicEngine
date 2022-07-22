#pragma once

#include <iostream>
#include <Eigen/Core>
#include "Mesh.h"

using namespace std;

namespace igl
{

namespace opengl
{

class Mesh
{
public:

	Mesh(const string& file);

//private:

	Eigen::MatrixXd V; // Vertices of the current mesh (#V x 3)
	Eigen::MatrixXi F; // Faces of the mesh (#F x 3)
	Eigen::MatrixXd V_normals; // One normal per vertex
	Eigen::MatrixXd V_uv; // UV vertices


};

} // opengl

} // igl
