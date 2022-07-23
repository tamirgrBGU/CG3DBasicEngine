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
	/* TAL: removing getter and setter as a first step to get rid of these ids
	void SetShader(const int id) { shaderID = id; }
	int BindProgram() const{
		return shaderID;
	}
	*/
  // Empty all fields
	void clear();

	// Change the visualization mode, invalidating the cache if necessary
	void set_face_based(bool newvalue);

	// Helpers that can draw the most common meshes
	void set_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);
	void set_vertices(const Eigen::MatrixXd& V);
	void set_normals(const Eigen::MatrixXd& N);

	void set_visible(bool value, unsigned int property_id = 1);

	// Set per-vertex UV coordinates
	//
	// Inputs:
	//   UV  #V by 2 list of UV coordinates (indexed by F)
	void set_uv(const Eigen::MatrixXd& UV);

	// Set per-corner UV coordinates
	//
	// Inputs:
	//   UV_V  #UV by 2 list of UV coordinates
	//   UV_F  #F by 3 list of UV indices into UV_V
	void set_uv(const Eigen::MatrixXd& UV_V, const Eigen::MatrixXi& UV_F);

	// Set the texture associated with the mesh.
	//
	// Inputs:
	//   R  width by height image matrix of red channel
	//   G  width by height image matrix of green channel
	//   B  width by height image matrix of blue channel
	//
	void set_texture( // TODO: TAL: remove?
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B);

	  // Set the texture associated with the mesh.
	  //
	  // Inputs:
	  //   R  width by height image matrix of red channel
	  //   G  width by height image matrix of green channel
	  //   B  width by height image matrix of blue channel
	  //   A  width by height image matrix of alpha channel
	  //
	void set_texture( // TODO: TAL: remove?
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B,
		const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& A);

	// Computes the normals of the mesh
	void compute_normals();

	  // Generate a normal image matcap
	void normal_matcap();

	// Generates a default grid texture (without uvs)
	void grid_texture();
  //  void image_texture(const std::string& fileName );

	// Copy visualization options from one viewport to another
	// void copy_options(const ViewerCore &from, const ViewerCore &to);
	Eigen::MatrixXd V; // Vertices of the current mesh (#V x 3)
	Eigen::MatrixXi F; // Faces of the mesh (#F x 3)

	// Per face attributes
	Eigen::MatrixXd F_normals; // One normal per face

	Eigen::MatrixXd F_material_ambient; // Per face ambient color
	Eigen::MatrixXd F_material_diffuse; // Per face diffuse color
	Eigen::MatrixXd F_material_specular; // Per face specular color

	// Per vertex attributes
	Eigen::MatrixXd V_normals; // One normal per vertex

	Eigen::MatrixXd V_material_ambient; // Per vertex ambient color
	Eigen::MatrixXd V_material_diffuse; // Per vertex diffuse color
	Eigen::MatrixXd V_material_specular; // Per vertex specular color

	// UV parametrization
	Eigen::MatrixXd V_uv; // UV vertices
	Eigen::MatrixXi F_uv; // optional faces for UVs

	// Texture // TODO: TAL: remove?
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_R;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_G;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_B;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> texture_A;

	// Overlays

	// Lines plotted over the scene
	// (Every row contains 9 doubles in the following format S_x, S_y, S_z, T_x, T_y, T_z, C_r, C_g, C_b),
	// with S and T the coordinates of the two vertices of the line in global coordinates, and C the color in floating point rgb format
	Eigen::MatrixXd lines;

	// Points plotted over the scene
	// (Every row contains 6 doubles in the following format P_x, P_y, P_z, C_r, C_g, C_b),
	// with P the position in global coordinates of the center of the point, and C the color in floating point rgb format
	Eigen::MatrixXd points;

	// Text labels plotted over the scene
	// Textp contains, in the i-th row, the position in global coordinates where the i-th label should be anchored
	// Texts contains in the i-th position the text of the i-th label
	Eigen::MatrixXd           vertex_labels_positions;
	Eigen::MatrixXd           face_labels_positions;
	Eigen::MatrixXd           labels_positions;
	std::vector<std::string>  vertex_labels_strings;
	std::vector<std::string>  face_labels_strings;
	std::vector<std::string>  labels_strings;

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
	unsigned int show_vertex_labels; // TODO: TAL: nice-to-have
	unsigned int show_face_labels; // TODO: TAL: nice-to-have
	unsigned int show_custom_labels; // TODO: TAL: nice-to-have
	unsigned int viewports;
	unsigned int type; // TODO: TAL: remove?
	bool hide; // TODO: TAL: is this the visibility thing?
	bool show_vertid; // shared across viewports for now
	bool show_faceid; // shared across viewports for now

	// Point size / line width
	float point_size;
	// line_width is NOT SUPPORTED on Mac OS and Windows
	float line_width;
	Eigen::Matrix<float, 4, 1, Eigen::DontAlign> line_color;
	Eigen::Matrix<float, 4, 1, Eigen::DontAlign> label_color;

	// Shape material
	float shininess; // TODO: TAL: should be in the material

	// Unique identifier
	int id; // TODO: TAL: remove?
	int shaderID; // TODO: TAL: remove?


	  // OpenGL representation of the mesh
	igl::opengl::MeshGL meshgl; // TODO: TAL: remove?

	// Update contents from a 'Data' instance
	void update_labels(
		igl::opengl::MeshGL& meshgl,
		igl::opengl::MeshGL::TextGL& GL_labels,
		const Eigen::MatrixXd& positions,
		const std::vector<std::string>& strings
	);
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
