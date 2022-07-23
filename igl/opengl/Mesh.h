#pragma once

#include <iostream>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

namespace igl
{

namespace opengl
{

class Mesh
{
public:

	Mesh(const string& file);

	// TODO: TAL: make these private
	const MatrixXd V; // Vertices of the current mesh (#V x 3)
	const MatrixXi F; // Faces of the mesh (#F x 3)
	const MatrixXd V_normals; // One normal per vertex
	const MatrixXd V_uv; // UV vertices

private:

	MeshGL meshgl;

	Mesh(const MatrixXd& V, const MatrixXi& F, const MatrixXd& V_normals, const MatrixXd& V_uv);
	static const Mesh LoadFile(const string& file);
};

} // opengl

} // igl
