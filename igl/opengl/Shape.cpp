#include "Shape.h"

namespace igl
{

namespace opengl
{

Shape::Shape(const string& file) : mesh(move(make_shared<Mesh>(file))) {}

Shape::Shape(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material)
	: mesh(move(mesh)), material(move(material)) {}



} // opengl

} // igl
