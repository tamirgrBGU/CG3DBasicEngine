// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#include "Viewer.h"

#include <thread>
#include <Eigen/LU>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cassert>

#include <igl/project.h>
#include <igl/readOBJ.h>
#include <igl/readOFF.h>
#include <igl/adjacency_list.h>
#include <igl/writeOBJ.h>
#include <igl/writeOFF.h>
#include <igl/massmatrix.h>
#include <igl/file_dialog_open.h>
#include <igl/file_dialog_save.h>
#include <igl/quat_mult.h>
#include <igl/axis_angle_to_quat.h>
#include <igl/trackball.h>
#include <igl/two_axis_valuator_fixed_up.h>
#include <igl/snap_to_canonical_view_quat.h>
#include <igl/unproject.h>
#include <igl/serialize.h>
#include "../gl.h"

using std::make_shared;
using namespace Eigen;

// Internal global variables used for glfw event handling
//static igl::opengl::glfw::Viewer * __viewer;
static double highdpi = 1;
static double scroll_x = 0;
static double scroll_y = 0;

namespace igl
{

namespace opengl
{

namespace glfw
{

Viewer::Viewer() :
	selected_data_index(0),
	next_data_id(1),
	next_shader_id(1),
	isActive(false),
	pickedShape(-1)
{
	//data_list.front() = new ViewerData(); // this was for the overylays
	//data_list.front()->id = 0;

	staticScene = 0;
	overlay_point_program = nullptr;
	overlay_program = nullptr;

	// Temporary variables initialization
	scroll_position = 0.0f;
}

Matrix4d Viewer::CalcParentsTrans(int indx)
{
	Matrix4d prevTrans = Matrix4d::Identity();

	for (int i = indx; parents[i] >= 0; i = parents[i])
	{
		prevTrans = data_list[parents[i]]->shape->MakeTransd() * prevTrans;
	}

	return prevTrans;
}


void Viewer::Draw(const Matrix4f& Proj, const Matrix4f& View, int viewportIndx, unsigned int flgs, unsigned int property_id)
{
	Matrix4f Normal;

	if (!(staticScene & (1 << viewportIndx)))
		Normal = MakeTransScale();
	else
		Normal = Matrix4f::Identity();

	for (int i = 0; i < data_list.size(); i++)
	{
		auto shape = data_list[i];
		auto material = shape->shape ? shape->shape->GetMaterial() : nullptr;
		if (shape->Is2Render(viewportIndx))
		{

			Matrix4f Model = shape->shape->MakeTransScale();

			if (!shape->shape->IsStatic())
			{

				Model = Normal * GetPreviousTrans(View.cast<double>(), i).cast<float>() * Model;
			}
			else if (parents[i] == -2) {
				Model = View.inverse() * Model;
			}
			if (!(flgs & 65536))
			{
				auto program = material->BindProgram();

				Update(Proj, View, Model, program);

				// Draw fill
				if (shape->show_faces & property_id)
					shape->Draw(program.get(), true);
				if (shape->shape->IsShowWireframe()) { // TODO: TAL: why was there | &property_id here?
					glLineWidth(shape->line_width);
					program = material->BindFixedColorProgram(); // TODO: TAL: temporary hack - change this
					Update(Proj, View, Model, program);
					program->SetUniform4f("fixedColor", 0.0f, 0.0f, 0.0f, 1.0f);
					shape->Draw(program.get(), false);
					program->SetUniform4f("fixedColor", 0.0f, 0.0f, 0.0f, 0.0f);
					program = material->BindProgram(); // TODO: TAL: temporary hack - change this
				}
				// overlay draws
				//if (shape->show_overlay & property_id) {
				//	if (shape->show_overlay_depth & property_id)
				//		glEnable(GL_DEPTH_TEST);
				//	else
				//		glDisable(GL_DEPTH_TEST);
				//	if (shape->lines.rows() > 0)
				//	{
				//		Update_overlay(Proj, View, Model, i, false);
				//		glEnable(GL_LINE_SMOOTH);
				//		overlay_program->Bind();
				//		shape->Draw_overlay(overlay_program, false);
				//	}
				//	if (shape->points.rows() > 0)
				//	{
				//		Update_overlay(Proj, View, Model, i, true);
				//		overlay_point_program->Bind();
				//		shape->Draw_overlay_points(overlay_point_program, false);
				//	}
				//	glEnable(GL_DEPTH_TEST);
				//}
			}
			else
			{ //picking
				auto program = material->BindFixedColorProgram();
				if (flgs & 8192) // TODO: TAL: check if this flag value is correct
				{
					Affine3f scale_mat = Affine3f::Identity();
					scale_mat.scale(Vector3f(1.1f, 1.1f, 1.1f));
					Update(Proj, View * Normal, Normal.inverse() * Model * scale_mat.matrix(), program);
				}
				else
				{
					Update(Proj, View * Normal, Normal.inverse() * Model, program);
				}
				shape->Draw(program.get(), true);
			}
		}
	}
}

ViewerData* Viewer::AddViewerData(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material, int parent, int viewport)
{
	auto d = new ViewerData();
	data_list.emplace_back(d);

	d->set_mesh(mesh->V, mesh->F);
	d->set_uv(mesh->V_uv);
	d->set_normals(mesh->V_normals);
	//d->compute_normals(); // TODO: TAL: does this needs to be called if V_normals is empty?
	// Elik: why?
	if (d->V_uv.rows() == 0)
	{
		d->grid_texture();
	}
	d->shape = make_shared<Shape>(move(mesh), move(material));
	d->type = MeshCopy;
	d->shaderID = 1;
	d->viewports = 1 << viewport;
	d->is_visible = true;
	d->hide = false;
	d->show_overlay = 0;

	this->parents.emplace_back(parent);

	return d;
}

string Viewer::ShapeTypeToFileName(int type) {
	switch (type) {
	case Plane:
		return "./data/plane.obj";
	case Cube:
		return "./data/cube.obj";
	case Octahedron:
		return "./data/octahedron.obj";
	case Tethrahedron:
		return "./data/Tetrahedron.obj";
	case Sphere:
		return "./data/sphere.obj";
	case Axis:
		return "./data/cube.obj";
	}
	throw runtime_error("Unknown shape type");
}

shared_ptr<Shape> Viewer::AddShapeFromFile(const std::string& file, shared_ptr<const Material> material, int parent, int viewport)
{
	shared_ptr<const Mesh>& mesh = make_shared<const Mesh>(file);
	auto d = AddViewerData(mesh, material, parent, viewport);
	return d->shape;
}

shared_ptr<Shape> Viewer::AddShapeFromMesh(shared_ptr<const Mesh> mesh, shared_ptr<const Material> material, int parent, int viewport)
{
	auto d = AddViewerData(mesh, material, parent, viewport);
	return d->shape;
}

shared_ptr<Shape> Viewer::AddShape(int type, shared_ptr<const Material> material, int parent, int viewport)
{
	shared_ptr<Mesh>& mesh = make_shared<Mesh>(ShapeTypeToFileName(type));
	auto d = AddViewerData(mesh, material, parent, viewport);
	d->type = type; // overwrite the value MeshCopy
	if (type == Axis) {
		d->is_visible = 0;
		d->show_faces = 0;
		d->show_overlay = 0xFF;
	}
	return d->shape;
}

void Viewer::ClearPickedShapes(int viewportIndx)
{
	for (int pShape : pShapes)
	{
		data_list[pShape]->RemoveViewport(viewportIndx);
	}
	pickedShape = -1;
	pShapes.clear();
}

//return coordinates in global system for a tip of arm position is local system
void Viewer::MouseProccessing(int button, int xrel, int yrel, float movCoeff, Matrix4d cameraMat, int viewportIndx)
{
	Matrix4d scnMat = Matrix4d::Identity();
	if (pickedShape <= 0 && !(staticScene & (1 << viewportIndx)))
		scnMat = MakeTransd().inverse();
	else if (!(staticScene & (1 << viewportIndx)))
		scnMat = (MakeTransd() * GetPreviousTrans(Matrix4d::Identity(), pickedShape)).inverse();
	else if (pickedShape > 0)
		scnMat = (GetPreviousTrans(Matrix4d::Identity(), pickedShape)).inverse();

	if (button == 1)
	{
		for (int pShape : pShapes)
		{
			pickedShape = pShape;
			WhenTranslate(scnMat * cameraMat, -xrel / movCoeff, yrel / movCoeff);
		}
	}
	else
	{
		movCoeff = 2.0f;

		if (button == 0)
		{
			//                if (pickedShape > 0 )
			WhenRotate(scnMat * cameraMat, -((float)xrel / 180) / movCoeff, ((float)yrel / 180) / movCoeff);

		}
		else
		{
			for (int pShape : pShapes)
			{
				pickedShape = pShape;
				WhenScroll(scnMat * cameraMat, yrel / movCoeff);
			}
		}
	}
}

bool Viewer::Picking(unsigned char data[4], int newViewportIndx)
{
	return false;
}

void Viewer::WhenTranslate(const Matrix4d& preMat, float dx, float dy)
{
	Movable* obj;
	if (pickedShape == -1 || data_list[pickedShape]->shape->IsStatic())
		obj = (Movable*)this;
	else { obj = (Movable*)data_list[pickedShape]; }
	obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3d(dx, 0, 0));
	obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3d(0, dy, 0));
	WhenTranslate(dx, dy);
}

void Viewer::WhenRotate(const Matrix4d& preMat, float dx, float dy)
{
	Movable* obj;
	if (pickedShape == -1 || data_list[pickedShape]->shape->IsStatic())
		obj = (Movable*)this;
	else
	{
		int ps = pickedShape;
		for (; parents[ps] > -1; ps = parents[ps]);
		obj = (Movable*)data_list[ps];
	}
	obj->RotateInSystem(dx, Axis::X);
	obj->RotateInSystem(dy, Axis::Y);
	WhenRotate(dx, dy);
}

void Viewer::WhenScroll(const Matrix4d& preMat, float dy)
{
	if (pickedShape == -1 || data_list[pickedShape]->shape->IsStatic())
		this->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3d(0, 0, dy));
	else
		data_list[pickedShape]->shape->TranslateInSystem(preMat.block<3, 3>(0, 0), Vector3d(0, 0, dy));
	WhenScroll(dy);
}

int Viewer::AddMaterial(Material* material)
{
	materials.push_back(material);
	return (materials.size() - 1);
}

Matrix4d Viewer::GetPreviousTrans(const Matrix4d& View, unsigned int index)
{
	Matrix4d Model = Matrix4d::Identity();
	int p = index >= 0 ? parents[index] : -1;
	for (; p >= 0; p = parents[p])
		Model = data_list[p]->shape->MakeTransd() * Model;
	if (p == -2)
		return  View.inverse() * Model;
	else
		return Model;
}

float Viewer::AddPickedShapes(const Matrix4d& PV, const Vector4i& viewport, int viewportIndx, int left, int right, int up, int bottom, int newViewportIndx)
{
	//not correct when the shape is scaled
	Matrix4d MVP = PV * MakeTransd();
	std::cout << "picked shapes  ";
	bool isFound = false;
	for (int i = 1; i < data_list.size(); i++)
	{ //add to pShapes if the center in range
		Matrix4d Model = data_list[i]->shape->MakeTransd();
		Model = CalcParentsTrans(i) * Model;
		Vector4d pos = MVP * Model * Vector4d(0, 0, 0, 1);
		float xpix = (1 + pos.x() / pos.z()) * viewport.z() / 2;
		float ypix = (1 + pos.y() / pos.z()) * viewport.w() / 2;
		if (data_list[i]->Is2Render(viewportIndx) && xpix < right && xpix > left && ypix < bottom && ypix > up)
		{
			pShapes.push_back(i);
			data_list[i]->AddViewport(newViewportIndx);
			std::cout << i << ", ";
			pickedShape = i;
			isFound = true;
		}
	}
	std::cout << std::endl;
	if (isFound)
	{
		Vector4d tmp = MVP * GetPreviousTrans(Matrix4d::Identity(), pickedShape) * data_list[pickedShape]->shape->MakeTransd() * Vector4d(0, 0, 1, 1);
		return (float)tmp.z();
	}
	else
		return -1;
}

int Viewer::AddTexture(const std::string& textureFileName, int dim)
{
	textures.push_back(new Texture(textureFileName, dim));
	return(textures.size() - 1);
}

int Viewer::AddTexture(int width, int height, unsigned char* data, int mode)
{
	if (mode)
		textures.push_back(new Texture(GL_DEPTH24_STENCIL8, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, data));
	else
		textures.push_back(new Texture(GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data)); //note GL_RED internal format, to save memory.

	return(textures.size() - 1);
}

void Viewer::Update_overlay(const Matrix4f& Proj, const Matrix4f& View, const Matrix4f& Model, unsigned int shapeIndx, bool is_points) {
	auto data = data_list[shapeIndx];
	Program* p = is_points ? overlay_point_program : overlay_program;
	if (p != nullptr) {
		p->Bind();
		p->SetUniformMatrix4f("Proj", &Proj);
		p->SetUniformMatrix4f("View", &View);
		p->SetUniformMatrix4f("Model", &Model);
	}
}

void Viewer::SetParent(int indx, int newValue, bool savePosition)
{
	parents[indx] = newValue;
	if (savePosition)
	{
		Vector4d tmp = data_list[newValue]->shape->MakeTransd() * (data_list[indx]->shape->MakeTransd()).inverse() * Vector4d(0, 0, 0, 1);
		data_list[indx]->shape->ZeroTrans();
		data_list[indx]->shape->Translate(-tmp.head<3>(), false);
	}
}

} // glfw

} // opengl

} // igl
