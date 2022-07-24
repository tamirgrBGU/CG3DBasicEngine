#include "Mesh.h"
#include "OBJ_Loader.h"

namespace igl
{

namespace opengl
{

Mesh::Mesh(const MatrixXd& V, const MatrixXi& F, const MatrixXd& V_normals, const MatrixXd& V_uv) :
	V(V), F(F), V_normals(V_normals), V_uv(V_uv) {}

Mesh::Mesh(const string& file) : 
	Mesh(LoadFile(file)) // using copy elision
{}

const Mesh Mesh::LoadFile(const string& file)
{
	objl::Loader loader;

	loader.LoadFile(file);

	// currently using only a single mesh (the first one loaded)
	const auto& vertices = loader.LoadedMeshes[0].Vertices;
	const auto& indices = loader.LoadedMeshes[0].Indices;

	MatrixXd V(vertices.size(), 3);
	MatrixXi F(indices.size() / 3, 3);
	MatrixXd V_normals(vertices.size(), 3);
	MatrixXd V_uv(vertices.size(), 2);

	// import the vertices data
	int i = 0;
	for (const auto& vertex : vertices)
	{
		V(i, 0) = vertex.Position.X;
		V(i, 1) = vertex.Position.Y;
		V(i, 2) = vertex.Position.Z;
		V_normals(i, 0) = vertex.Normal.X;
		V_normals(i, 1) = vertex.Normal.Y;
		V_normals(i, 2) = vertex.Normal.Z;
		V_uv(i, 0) = vertex.TextureCoordinate.X;
		V_uv(i, 1) = vertex.TextureCoordinate.Y;
		i++;
	}

	// import the faces data
	F.resize(indices.size() / 3, 3);

	i = 0;
	for (int index : indices)
	{
		F(i / 3, i % 3) = index;
		i++;
	}

	// TODO: TAL: add face normals?

	return Mesh(V, F, V_normals, V_uv);
}

} // opengl

} // igl
