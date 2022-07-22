#include "Mesh.h"
#include "OBJ_Loader.h"

namespace igl
{

namespace opengl
{

Mesh::Mesh(const string& file)
{
	objl::Loader loader;

	loader.LoadFile(file);

	// currently using only a single mesh (the first one loaded)

	// import the vertices data
	auto& vertices = loader.LoadedMeshes[0].Vertices;
	V.resize(vertices.size(), 3);
	V_normals.resize(vertices.size(), 3);
	V_uv.resize(vertices.size(), 2);
	int i = 0;
	for (auto& vertex : vertices)
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
	auto& indices = loader.LoadedMeshes[0].Indices;
	F.resize(indices.size() / 3, 3);

	i = 0; // bahhh
	for (int index : indices)
	{
		F(i / 3, i % 3) = index;
		i++;
	}

	// TODO: TAL: add face normals

}

} // opengl

} // igl
