// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#include "ViewerData.h"

#include "../per_face_normals.h"
#include "../material_colors.h"
#include "../parula.h"
#include "../per_vertex_normals.h"

#include "igl/png/texture_from_png.h"
// Really? Just for GL_NEAREST?
#include "gl.h"
#include "igl/parula.h"

#include <iostream>

namespace igl
{

namespace opengl
{

ViewerData::ViewerData()
	: dirty(MeshGL::DIRTY_ALL),
	show_faces(~unsigned(0)),
	face_based(false),
	double_sided(false),
	invert_normals(false),
	show_overlay(~unsigned(0)),
	show_overlay_depth(~unsigned(0)),
	show_vertex_labels(0),
	show_face_labels(0),
	show_custom_labels(0),
	show_texture(false),
	use_matcap(false),
	point_size(30),
	line_width(1.5f),
	line_color(0, 0, 0, 1),
	label_color(0, 0, 0.04, 1),
	shininess(35.0f),
	id(-1),
	is_visible(~unsigned(0)),
	materialID(0),
	isCopy(false),
	isStatic(false)
{
	clear();
};

void ViewerData::set_face_based(bool newvalue)
{
	if (face_based != newvalue)
	{
		face_based = newvalue;
		dirty = MeshGL::DIRTY_ALL;
	}
}

// Helpers that draws the most common meshes
void ViewerData::set_mesh(const Eigen::MatrixXd& _V, const Eigen::MatrixXi& _F)
{
	using namespace std;

	Eigen::MatrixXd V_temp;

	// If V only has two columns, pad with a column of zeros
	if (_V.cols() == 2)
	{
		V_temp = Eigen::MatrixXd::Zero(_V.rows(), 3);
		V_temp.block(0, 0, _V.rows(), 2) = _V;
	}
	else
		V_temp = _V;

	if (V.rows() == 0 && F.rows() == 0)
	{
		V = V_temp;
		F = _F;

		compute_normals();
		grid_texture();
	}
	else
	{
		if (_V.rows() == V.rows() && _F.rows() == F.rows())
		{
			V = V_temp;
			F = _F;
		}
		else
			cerr << "ERROR (set_mesh): The new mesh has a different number of vertices/faces. Please clear the mesh before plotting." << endl;
	}
	dirty |= MeshGL::DIRTY_FACE | MeshGL::DIRTY_POSITION;
}

void ViewerData::set_vertices(const Eigen::MatrixXd& _V)
{
	V = _V;
	assert(F.size() == 0 || F.maxCoeff() < V.rows());
	dirty |= MeshGL::DIRTY_POSITION;
}

void ViewerData::set_normals(const Eigen::MatrixXd& N)
{
	using namespace std;
	if (N.rows() == V.rows())
	{
		set_face_based(false);
		V_normals = N;
	}
	else if (N.rows() == F.rows() || N.rows() == F.rows() * 3)
	{
		set_face_based(true);
		F_normals = N;
	}
	else
		cerr << "ERROR (set_normals): Please provide a normal per face, per corner or per vertex." << endl;
	dirty |= MeshGL::DIRTY_NORMAL;
}

void ViewerData::set_visible(bool value, unsigned int property_id /*= 1*/)
{
	if (value)
		is_visible |= property_id;
	else
		is_visible &= ~property_id;
}

void ViewerData::set_uv(const Eigen::MatrixXd& UV)
{
	using namespace std;
	if (UV.rows() == V.rows())
	{
		set_face_based(false);
		V_uv = UV;
	}
	else
		cerr << "ERROR (set_UV): Please provide uv per vertex." << endl;
	dirty |= MeshGL::DIRTY_UV;
}

void ViewerData::set_uv(const Eigen::MatrixXd& UV_V, const Eigen::MatrixXi& UV_F)
{
	set_face_based(true);
	V_uv = UV_V.block(0, 0, UV_V.rows(), 2);
	F_uv = UV_F;
	dirty |= MeshGL::DIRTY_UV;
}

void ViewerData::set_texture(
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B)
{
	texture_R = R;
	texture_G = G;
	texture_B = B;
	texture_A = Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>::Constant(R.rows(), R.cols(), 255);
	dirty |= MeshGL::DIRTY_TEXTURE;
}

void ViewerData::set_texture(
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& R,
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& G,
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& B,
	const Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>& A)
{
	texture_R = R;
	texture_G = G;
	texture_B = B;
	texture_A = A;
	dirty |= MeshGL::DIRTY_TEXTURE;
}

void ViewerData::clear()
{
	V = Eigen::MatrixXd(0, 3);
	F = Eigen::MatrixXi(0, 3);

	F_material_ambient = Eigen::MatrixXd(0, 4);
	F_material_diffuse = Eigen::MatrixXd(0, 4);
	F_material_specular = Eigen::MatrixXd(0, 4);

	V_material_ambient = Eigen::MatrixXd(0, 4);
	V_material_diffuse = Eigen::MatrixXd(0, 4);
	V_material_specular = Eigen::MatrixXd(0, 4);

	F_normals = Eigen::MatrixXd(0, 3);
	V_normals = Eigen::MatrixXd(0, 3);

	V_uv = Eigen::MatrixXd(0, 2);
	F_uv = Eigen::MatrixXi(0, 3);

	lines = Eigen::MatrixXd(0, 9);
	points = Eigen::MatrixXd(0, 6);

	vertex_labels_positions = Eigen::MatrixXd(0, 3);
	face_labels_positions = Eigen::MatrixXd(0, 3);
	labels_positions = Eigen::MatrixXd(0, 3);
	vertex_labels_strings.clear();
	face_labels_strings.clear();
	labels_strings.clear();

	face_based = false;
	double_sided = false;
	invert_normals = false;
	show_texture = false;
	use_matcap = false;
}

void ViewerData::compute_normals()
{
	if (V.cols() == 2)
	{
		F_normals = Eigen::RowVector3d(0, 0, 1).replicate(F.rows(), 1);
		V_normals = Eigen::RowVector3d(0, 0, 1).replicate(V.rows(), 1);
	}
	else
	{
		assert(V.cols() == 3);
		igl::per_face_normals(V, F, F_normals);
		igl::per_vertex_normals(V, F, F_normals, V_normals);
	}
	dirty |= MeshGL::DIRTY_NORMAL;
}

void ViewerData::normal_matcap()
{
	const int size = 512;
	texture_R.resize(size, size);
	texture_G.resize(size, size);
	texture_B.resize(size, size);
	const Eigen::Vector3d navy(0.3, 0.3, 0.5);
	static const auto clamp = [](double t) { return std::max(std::min(t, 1.0), 0.0); };
	for (int i = 0; i < size; i++)
	{
		const double x = (double(i) / double(size - 1) * 2. - 1.);
		for (int j = 0; j < size; j++)
		{
			const double y = (double(j) / double(size - 1) * 2. - 1.);
			const double z = sqrt(1.0 - std::min(x * x + y * y, 1.0));
			Eigen::Vector3d C = Eigen::Vector3d(x * 0.5 + 0.5, y * 0.5 + 0.5, z);
			texture_R(i, j) = clamp(C(0)) * 255;
			texture_G(i, j) = clamp(C(1)) * 255;
			texture_B(i, j) = clamp(C(2)) * 255;
		}
	}
	texture_A.setConstant(texture_R.rows(), texture_R.cols(), 255);
	dirty |= MeshGL::DIRTY_TEXTURE;
}

void ViewerData::grid_texture()
{
  // Don't do anything for an empty mesh
	if (V.rows() == 0)
	{
		V_uv.resize(V.rows(), 2);
		return;
	}
	if (V_uv.rows() == 0)
	{
		V_uv = V.block(0, 0, V.rows(), 2);
		V_uv.col(0) = V_uv.col(0).array() - V_uv.col(0).minCoeff();
		V_uv.col(0) = V_uv.col(0).array() / V_uv.col(0).maxCoeff();
		V_uv.col(1) = V_uv.col(1).array() - V_uv.col(1).minCoeff();
		V_uv.col(1) = V_uv.col(1).array() / V_uv.col(1).maxCoeff();
		V_uv = V_uv.array() * 10;
		dirty |= MeshGL::DIRTY_TEXTURE;
	}

	unsigned size = 4;
	unsigned size2 = size / 2;
	texture_R.resize(size, size);
	for (unsigned i = 0; i < size; ++i)
	{
		for (unsigned j = 0; j < size; ++j)
		{
			texture_R(i, j) = 0;
			if ((i < size2 && j < size2) || (i >= size2 && j >= size2))
				texture_R(i, j) = 255;
		}
	}

	texture_G = texture_R;
	texture_B = texture_R;
	texture_A = Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic>::Constant(texture_R.rows(), texture_R.cols(), 255);
	dirty |= MeshGL::DIRTY_TEXTURE;
}

// Populate VBOs of a particular label stype (Vert, Face, Custom)
void ViewerData::update_labels(
	MeshGL& meshgl,
	MeshGL::TextGL& GL_labels,
	const Eigen::MatrixXd& positions,
	const std::vector<std::string>& strings
) {
	if (positions.rows() > 0)
	{
		int numCharsToRender = 0;
		for (size_t p = 0; p < positions.rows(); p++)
		{
			numCharsToRender += strings.at(p).length();
		}
		GL_labels.label_pos_vbo.resize(numCharsToRender, 3);
		GL_labels.label_char_vbo.resize(numCharsToRender, 1);
		GL_labels.label_offset_vbo.resize(numCharsToRender, 1);
		GL_labels.label_indices_vbo.resize(numCharsToRender, 1);
		int idx = 0;
		assert(strings.size() == positions.rows());
		for (size_t s = 0; s < strings.size(); s++)
		{
			const auto& label = strings.at(s);
			for (size_t c = 0; c < label.length(); c++)
			{
				GL_labels.label_pos_vbo.row(idx) = positions.row(s).cast<float>();
				GL_labels.label_char_vbo(idx) = (float)(label.at(c));
				GL_labels.label_offset_vbo(idx) = c;
				GL_labels.label_indices_vbo(idx) = idx;
				idx++;
			}
		}
	}
}

void ViewerData::updateGL(
	const bool invert_normals,
	MeshGL& meshgl
)
{
	if (!meshgl.is_initialized)
	{
		meshgl.init();
	}

	bool per_corner_uv = (this->F_uv.rows() == this->F.rows());
	bool per_corner_normals = (this->F_normals.rows() == 3 * this->F.rows());

	meshgl.dirty |= this->dirty;
	meshgl.dirty_mesh_shader = MeshGL::DirtyFlags::DIRTY_ALL;

	// Input:
	//   X  #F by dim quantity
	// Output:
	//   X_vbo  #F*3 by dim scattering per corner
	const auto per_face = [this](
		const Eigen::MatrixXd& X,
		MeshGL::RowMatrixXf& X_vbo)
	{
		assert(X.cols() == 4);
		X_vbo.resize(this->F.rows() * 3, 4);
		for (unsigned i = 0; i < this->F.rows(); ++i)
			for (unsigned j = 0; j < 3; ++j)
				X_vbo.row(i * 3 + j) = X.row(i).cast<float>();
	};

	const auto per_face_duplicate = [this](
		const Eigen::MatrixXf& X,
		MeshGL::RowMatrixXf& X_vbo)
	{
		assert(X.cols() == 4);
		X_vbo.resize(this->F.rows() * 3, 4);
		for (unsigned i = 0; i < this->F.rows(); ++i)
			for (unsigned j = 0; j < 3; ++j)
				X_vbo.row(i * 3 + j) = X;
	};

	// Input:
	//   X  #V by dim quantity
	// Output:
	//   X_vbo  #F*3 by dim scattering per corner
	const auto per_corner = [this](
		const Eigen::MatrixXd& X,
		MeshGL::RowMatrixXf& X_vbo)
	{
		X_vbo.resize(this->F.rows() * 3, X.cols());
		for (unsigned i = 0; i < this->F.rows(); ++i)
			for (unsigned j = 0; j < 3; ++j)
				X_vbo.row(i * 3 + j) = X.row(this->F(i, j)).cast<float>();
	};

	const auto per_corner_duplicate = [this](
		const Eigen::MatrixXf& X,
		MeshGL::RowMatrixXf& X_vbo)
	{
		X_vbo.resize(this->F.rows() * 3, X.cols());
		for (unsigned i = 0; i < this->F.rows(); ++i)
			for (unsigned j = 0; j < 3; ++j)
				X_vbo.row(i * 3 + j) = X;
	};
	
	if (!this->face_based)
	{
		if (!(per_corner_uv || per_corner_normals))
		{
		  // Vertex positions
			if (meshgl.dirty & MeshGL::DIRTY_POSITION)
				meshgl.V_vbo = this->V.cast<float>();

			  // Vertex normals
			if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
			{
				meshgl.V_normals_vbo = this->V_normals.cast<float>();
				if (invert_normals)
					meshgl.V_normals_vbo = -meshgl.V_normals_vbo;
			}

			  // Face indices
			if (meshgl.dirty & MeshGL::DIRTY_FACE)
				meshgl.F_vbo = this->F.cast<unsigned>();

			  // Texture coordinates
			if (meshgl.dirty & MeshGL::DIRTY_UV)
			{
				meshgl.V_uv_vbo = this->V_uv.cast<float>();
			}
		}
		else
		{

		  // Per vertex properties with per corner UVs
			if (meshgl.dirty & MeshGL::DIRTY_POSITION)
			{
				per_corner(this->V, meshgl.V_vbo);
			}

			if (meshgl.dirty & MeshGL::DIRTY_AMBIENT)
			{
				meshgl.V_ambient_vbo.resize(this->F.rows() * 3, 4);
				per_corner_duplicate(shape->GetAmbient(), meshgl.V_ambient_vbo);
			}
			if (meshgl.dirty & MeshGL::DIRTY_DIFFUSE)
			{
				meshgl.V_diffuse_vbo.resize(this->F.rows() * 3, 4);
				per_corner_duplicate(shape->GetDiffuse(), meshgl.V_diffuse_vbo);
			}
			if (meshgl.dirty & MeshGL::DIRTY_SPECULAR)
			{
				meshgl.V_specular_vbo.resize(this->F.rows() * 3, 4);
				per_corner_duplicate(shape->GetSpecular(), meshgl.V_specular_vbo);
			}

			if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
			{
				meshgl.V_normals_vbo.resize(this->F.rows() * 3, 3);
				per_corner(this->V_normals, meshgl.V_normals_vbo);

				if (invert_normals)
					meshgl.V_normals_vbo = -meshgl.V_normals_vbo;
			}

			if (meshgl.dirty & MeshGL::DIRTY_FACE)
			{
				meshgl.F_vbo.resize(this->F.rows(), 3);
				for (unsigned i = 0; i < this->F.rows(); ++i)
					meshgl.F_vbo.row(i) << i * 3 + 0, i * 3 + 1, i * 3 + 2;
			}

			if ((meshgl.dirty & MeshGL::DIRTY_UV) && this->V_uv.rows() > 0)
			{
				meshgl.V_uv_vbo.resize(this->F.rows() * 3, 2);
				for (unsigned i = 0; i < this->F.rows(); ++i)
					for (unsigned j = 0; j < 3; ++j)
						meshgl.V_uv_vbo.row(i * 3 + j) =
						this->V_uv.row(per_corner_uv ?
							this->F_uv(i, j) : this->F(i, j)).cast<float>();
			}
		}
	}
	else
	{
		if (meshgl.dirty & MeshGL::DIRTY_POSITION)
		{
			per_corner(this->V, meshgl.V_vbo);
		}
		if (meshgl.dirty & MeshGL::DIRTY_AMBIENT)
		{
			per_face_duplicate(shape->GetAmbient(), meshgl.V_ambient_vbo);
		}
		if (meshgl.dirty & MeshGL::DIRTY_DIFFUSE)
		{
			per_face_duplicate(shape->GetDiffuse(), meshgl.V_diffuse_vbo);
		}
		if (meshgl.dirty & MeshGL::DIRTY_SPECULAR)
		{
			per_face_duplicate(shape->GetSpecular(), meshgl.V_specular_vbo);
		}

		if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
		{
			meshgl.V_normals_vbo.resize(this->F.rows() * 3, 3);
			for (unsigned i = 0; i < this->F.rows(); ++i)
				for (unsigned j = 0; j < 3; ++j)
					meshgl.V_normals_vbo.row(i * 3 + j) =
					per_corner_normals ?
					this->F_normals.row(i * 3 + j).cast<float>() :
					this->F_normals.row(i).cast<float>();

			if (invert_normals)
				meshgl.V_normals_vbo = -meshgl.V_normals_vbo;
		}

		if (meshgl.dirty & MeshGL::DIRTY_FACE)
		{
			meshgl.F_vbo.resize(this->F.rows(), 3);
			for (unsigned i = 0; i < this->F.rows(); ++i)
				meshgl.F_vbo.row(i) << i * 3 + 0, i * 3 + 1, i * 3 + 2;
		}

		if ((meshgl.dirty & MeshGL::DIRTY_UV) && this->V_uv.rows() > 0)
		{
			meshgl.V_uv_vbo.resize(this->F.rows() * 3, 2);
			for (unsigned i = 0; i < this->F.rows(); ++i)
				for (unsigned j = 0; j < 3; ++j)
					meshgl.V_uv_vbo.row(i * 3 + j) = this->V_uv.row(per_corner_uv ? this->F_uv(i, j) : this->F(i, j)).cast<float>();
		}
	}

	if (meshgl.dirty & MeshGL::DIRTY_OVERLAY_LINES)
	{
		meshgl.lines_V_vbo.resize(this->lines.rows() * 2, 3);
		meshgl.lines_V_colors_vbo.resize(this->lines.rows() * 2, 3);
		meshgl.lines_F_vbo.resize(this->lines.rows() * 2, 1);
		for (unsigned i = 0; i < this->lines.rows(); ++i)
		{
			meshgl.lines_V_vbo.row(2 * i + 0) = this->lines.block<1, 3>(i, 0).cast<float>();
			meshgl.lines_V_vbo.row(2 * i + 1) = this->lines.block<1, 3>(i, 3).cast<float>();
			meshgl.lines_V_colors_vbo.row(2 * i + 0) = this->lines.block<1, 3>(i, 6).cast<float>();
			meshgl.lines_V_colors_vbo.row(2 * i + 1) = this->lines.block<1, 3>(i, 6).cast<float>();
			meshgl.lines_F_vbo(2 * i + 0) = 2 * i + 0;
			meshgl.lines_F_vbo(2 * i + 1) = 2 * i + 1;
		}
	}

	if (meshgl.dirty & MeshGL::DIRTY_OVERLAY_POINTS)
	{
		meshgl.points_V_vbo.resize(this->points.rows(), 3);
		meshgl.points_V_colors_vbo.resize(this->points.rows(), 3);
		meshgl.points_F_vbo.resize(this->points.rows(), 1);
		for (unsigned i = 0; i < this->points.rows(); ++i)
		{
			meshgl.points_V_vbo.row(i) = this->points.block<1, 3>(i, 0).cast<float>();
			meshgl.points_V_colors_vbo.row(i) = this->points.block<1, 3>(i, 3).cast<float>();
			meshgl.points_F_vbo(i) = i;
		}
	}

	if (meshgl.dirty & MeshGL::DIRTY_FACE_LABELS)
	{
		if (face_labels_positions.rows() == 0)
		{
			face_labels_positions.conservativeResize(F.rows(), 3);
			Eigen::MatrixXd faceNormals = F_normals.normalized();
			for (int f = 0; f < F.rows(); ++f)
			{
				std::string faceName = std::to_string(f);
				face_labels_positions.row(f) = V.row(F.row(f)(0));
				face_labels_positions.row(f) += V.row(F.row(f)(1));
				face_labels_positions.row(f) += V.row(F.row(f)(2));
				face_labels_positions.row(f) /= 3.;
				face_labels_positions.row(f) = (faceNormals * 0.05).row(f) + face_labels_positions.row(f);
				face_labels_strings.push_back(faceName);
			}
		}
		update_labels(
			meshgl,
			meshgl.face_labels,
			face_labels_positions,
			face_labels_strings
		);
	}

	if (meshgl.dirty & MeshGL::DIRTY_VERTEX_LABELS)
	{
		if (vertex_labels_positions.rows() == 0)
		{
			vertex_labels_positions.conservativeResize(V.rows(), 3);
			Eigen::MatrixXd normalized = V_normals.normalized();
			for (int v = 0; v < V.rows(); ++v)
			{
				std::string vertName = std::to_string(v);
				vertex_labels_positions.row(v) = (normalized * 0.1).row(v) + V.row(v);
				vertex_labels_strings.push_back(vertName);
			}
		}
		update_labels(
			meshgl,
			meshgl.vertex_labels,
			vertex_labels_positions,
			vertex_labels_strings
		);
	}

	if (meshgl.dirty & MeshGL::DIRTY_CUSTOM_LABELS)
	{
		update_labels(
			meshgl,
			meshgl.custom_labels,
			labels_positions,
			labels_strings
		);
	}
}

void ViewerData::Draw(const Program* program, bool solid) {
	/* Bind and potentially refresh mesh/line/point data */
	if (dirty)
	{
		updateGL(invert_normals, meshgl);
		dirty = MeshGL::DIRTY_NONE;
	}
	meshgl.bind_mesh(program->GetHandle(), program->GetId());
	meshgl.draw_mesh(solid);
}

void ViewerData::Draw_overlay(Program* program, bool cond) {
	if (dirty)
	{
		updateGL(invert_normals, meshgl);
		dirty = MeshGL::DIRTY_NONE;
	}
	if (program) {
		glLineWidth(this->line_width);
		meshgl.bind_overlay_lines(program->GetHandle(), program->GetId());
		meshgl.draw_overlay_lines();
	}
}

void ViewerData::Draw_overlay_points(Program* program, bool cond) {
	if (dirty)
	{
		updateGL(invert_normals, meshgl);
		dirty = MeshGL::DIRTY_NONE;
	}
	if (program) {
		glPointSize(this->point_size);
		meshgl.bind_overlay_points(program->GetHandle(), program->GetId());
		meshgl.draw_overlay_points();
	}
}

} // opengl

} // igl