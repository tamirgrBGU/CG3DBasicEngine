#pragma once

#include "MeshGL.h"
#include "glfw/VertexArray.hpp"
#include "Program.h"
#include <igl/igl_inline.h>
#include <igl/colormap.h>
#include <cassert>
#include <cstdint>
#include "Movable.h"
#include <Eigen/Core>
#include <memory>
#include <vector>
#include <igl/opengl/Shape.h>

// Alec: This is a mesh class containing a variety of data types (normals,
// overlays, material colors, etc.)
//
// WARNING: Eigen data members (such as Eigen::Vector4f) should explicitly
// disable alignment (e.g. use `Eigen::Matrix<float, 4, 1, Eigen::DontAlign>`),
// in order to avoid alignment issues further down the line (esp. if the
// structure are stored in a std::vector).
//
// See this thread for a more detailed discussion:
// https://github.com/libigl/libigl/pull/1029
//
namespace igl
{

namespace opengl
{

// Forward declaration
class ViewerCore;

class ViewerData
{
private:
	unsigned int materialID;
	bool isCopy;
	bool isStatic;

public:

	shared_ptr<Shape> shape;

	ViewerData();
	void Draw(const Program* program, bool solid);
	void clear();
	void set_face_based(bool newvalue);
	void set_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);
	void set_normals(const Eigen::MatrixXd& N);
	void set_visible(bool value, unsigned int property_id = 1);
	void set_uv(const Eigen::MatrixXd& UV);
	void set_uv(const Eigen::MatrixXd& UV_V, const Eigen::MatrixXi& UV_F);
	void set_texture( // TODO: TAL: remove?
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B);

	void set_texture( // TODO: TAL: remove?
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& A);
	void compute_normals();
	void normal_matcap();
	void grid_texture();

	Eigen::MatrixXd V; // Vertices of the current mesh (#V x 3)
	Eigen::MatrixXi F; // Faces of the mesh (#F x 3)
	Eigen::MatrixXd F_normals; // One normal per face
	Eigen::MatrixXd F_material_ambient; // Per face ambient color
	Eigen::MatrixXd F_material_diffuse; // Per face diffuse color
	Eigen::MatrixXd F_material_specular; // Per face specular color
	Eigen::MatrixXd V_normals; // One normal per vertex
	Eigen::MatrixXd V_material_ambient; // Per vertex ambient color
	Eigen::MatrixXd V_material_diffuse; // Per vertex diffuse color
	Eigen::MatrixXd V_material_specular; // Per vertex specular color
	Eigen::MatrixXd V_uv; // UV vertices
	Eigen::MatrixXi F_uv; // optional faces for UVs

	// Texture // TODO: TAL: remove?
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_R;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_G;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_B;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_A;

	// Overlays
	Eigen::MatrixXd lines;
	Eigen::MatrixXd points;

	// Marks dirty buffers that need to be uploaded to OpenGL
	uint32_t dirty;

	// Enable per-face or per-vertex properties
	bool face_based;

	// Enable double-sided lighting on faces
	bool double_sided; // TODO: TAL: nice-to-have

	// Invert mesh normals
	bool invert_normals;

	// Visualization options
	// Each option is a binary mask specifying on which viewport each option is set.
	// When using a single viewport, standard boolean can still be used for simplicity.
	unsigned int is_visible; // TODO: TAL: is this visibility per viewport? should be bool
	unsigned int show_overlay;
	unsigned int show_overlay_depth; // TODO: TAL: remove
	unsigned int show_texture; // TODO: TAL: remove
	unsigned int use_matcap; // ?
	unsigned int show_faces; // TODO: TAL: nice-to-have
	unsigned int viewports;
	unsigned int type; // TODO: TAL: remove?
	bool hide; // TODO: TAL: is this the visibility thing?
	bool show_vertid; // shared across viewports for now
	bool show_faceid; // shared across viewports for now

	float point_size;
	float line_width;
	Eigen::Matrix<float, 4, 1, Eigen::DontAlign> line_color;

	float shininess; // TODO: TAL: should be in the material

	// Unique identifier
	int id; // TODO: TAL: remove?
	int shaderID; // TODO: TAL: remove?

	igl::opengl::MeshGL meshgl; // TODO: TAL: remove?

	void updateGL(const bool invert_normals, igl::opengl::MeshGL& meshgl);
	void AddViewport(int viewport) { viewports = viewports | (1 << viewport); } // TODO: TAL: maybe move
	void RemoveViewport(int viewport) { viewports = viewports & ~(1 << viewport); } // TODO: TAL: maybe move
	bool Is2Render(int viewport) { return  (viewports & (1 << viewport)) && !hide; } // TODO: TAL: maybe move
	inline void Hide() { hide = true; }
	inline void UnHide() { hide = false; }
	void Draw_overlay(Program* program, bool cond);
	void Draw_overlay_points(Program* program, bool cond);
};

} // namespace opengl

} // namespace igl
