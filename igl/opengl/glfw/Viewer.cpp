// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#include "Viewer.h"

//#include <chrono>
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
//#include <igl/get_seconds.h>
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

  void Viewer::Init(const std::string config)
  {
	  

  }

  IGL_INLINE Viewer::Viewer():
    data_list(1),
    selected_data_index(0),
    next_data_id(1),
    next_shader_id(1),
	isActive(false),
    pickedShape(-1)
  {
    data_list.front() = new ViewerData();
    data_list.front()->id = 0;
    staticScene = 0;
    overlay_point_shader = nullptr;
    overlay_shader = nullptr;


    // Temporary variables initialization
   // down = false;
  //  hack_never_moved = true;
    scroll_position = 0.0f;
    SetShader_overlay("shaders/overlay");
    SetShader_point_overlay("shaders/overlay_points");

    // Per face
    data()->set_face_based(false);

    
//#ifndef IGL_VIEWER_VIEWER_QUIET
//    const std::string usage(R"(igl::opengl::glfw::Viewer usage:
//  [drag]  Rotate scene
//  A,a     Toggle animation (tight draw loop)
//  F,f     Toggle face based
//  I,i     Toggle invert normals
//  L,l     Toggle wireframe
//  O,o     Toggle orthographic/perspective projection
//  T,t     Toggle filled faces
//  [,]     Toggle between cameras
//  1,2     Toggle between models
//  ;       Toggle vertex labels
//  :       Toggle face labels)"
//);
//    std::cout<<usage<<std::endl;
//#endif
  }

  IGL_INLINE Viewer::~Viewer()
  {
  }
IGL_INLINE bool
    Viewer::load_mesh_from_data(const Eigen::MatrixXd &V,
                                const Eigen::MatrixXi &F,
                                const Eigen::MatrixXd &UV_V,
                                const Eigen::MatrixXi &UV_F) {
    if(!(data()->F.rows() == 0  && data()->V.rows() == 0))
    {
        append_mesh();
    }
    data()->clear();
        data()->set_mesh(V,F);
    if (UV_V.rows() > 0)
    {
        data()->set_uv(UV_V, UV_F);
    }
    else
    {
        data()->grid_texture();
    }
    data()->compute_normals();
    data()->uniform_colors(Eigen::Vector3d(255.0/255.0,255.0/255.0,0.0/255.0),
                           Eigen::Vector3d(255.0/255.0,228.0/255.0,58.0/255.0),
                           Eigen::Vector3d(255.0/255.0,235.0/255.0,80.0/255.0));
    return true;
  }
  IGL_INLINE bool Viewer::load_mesh_from_file(
      const std::string & mesh_file_name_string)
  {

    // Create new data slot and set to selected
    if(!(data()->F.rows() == 0  && data()->V.rows() == 0))
    {
      append_mesh();
    }
    data()->clear();

    size_t last_dot = mesh_file_name_string.rfind('.');
    if (last_dot == std::string::npos)
    {
      std::cerr<<"Error: No file extension found in "<<
        mesh_file_name_string<<std::endl;
      return false;
    }

    std::string extension = mesh_file_name_string.substr(last_dot+1);

    if (extension == "off" || extension =="OFF")
    {
      Eigen::MatrixXd V;
      Eigen::MatrixXi F;
      if (!igl::readOFF(mesh_file_name_string, V, F))
        return false;
      data()->set_mesh(V,F);
    }
    else if (extension == "obj" || extension =="OBJ")
    {
      Eigen::MatrixXd corner_normals;
      Eigen::MatrixXi fNormIndices;

      Eigen::MatrixXd UV_V;
      Eigen::MatrixXi UV_F;
      Eigen::MatrixXd V;
      Eigen::MatrixXi F;

      if (!(
            igl::readOBJ(
              mesh_file_name_string,
              V, UV_V, corner_normals, F, UV_F, fNormIndices)))
      {
        return false;
      }

      data()->set_mesh(V,F);
      if (UV_V.rows() > 0)
      {
          data()->set_uv(UV_V, UV_F);
      }

    }
    else
    {
      // unrecognized file type
      printf("Error: %s is not a recognized file type.\n",extension.c_str());
      return false;
    }

    data()->compute_normals();
    data()->uniform_colors(Eigen::Vector3d(255.0/255.0,255.0/255.0,0.0/255.0),
                   Eigen::Vector3d(255.0/255.0,228.0/255.0,58.0/255.0),
                   Eigen::Vector3d(255.0/255.0,235.0/255.0,80.0/255.0));

    // Elik: why?
    if (data()->V_uv.rows() == 0)
    {
      data()->grid_texture();
    }
    

    //for (unsigned int i = 0; i<plugins.size(); ++i)
    //  if (plugins[i]->post_load())
    //    return true;

    return true;
  }

  IGL_INLINE bool Viewer::save_mesh_to_file(
      const std::string & mesh_file_name_string)
  {
    // first try to load it with a plugin
    //for (unsigned int i = 0; i<plugins.size(); ++i)
    //  if (plugins[i]->save(mesh_file_name_string))
    //    return true;

    size_t last_dot = mesh_file_name_string.rfind('.');
    if (last_dot == std::string::npos)
    {
      // No file type determined
      std::cerr<<"Error: No file extension found in "<<
        mesh_file_name_string<<std::endl;
      return false;
    }
    std::string extension = mesh_file_name_string.substr(last_dot+1);
    if (extension == "off" || extension =="OFF")
    {
      return igl::writeOFF(
        mesh_file_name_string,data()->V,data()->F);
    }
    else if (extension == "obj" || extension =="OBJ")
    {
      Eigen::MatrixXd corner_normals;
      Eigen::MatrixXi fNormIndices;

      Eigen::MatrixXd UV_V;
      Eigen::MatrixXi UV_F;

      return igl::writeOBJ(mesh_file_name_string,
          data()->V,
          data()->F,
          corner_normals, fNormIndices, UV_V, UV_F);
    }
    else
    {
      // unrecognized file type
      printf("Error: %s is not a recognized file type.\n",extension.c_str());
      return false;
    }
    return true;
  }
 
  IGL_INLINE bool Viewer::load_scene()
  {
    std::string fname = igl::file_dialog_open();
    if(fname.length() == 0)
      return false;
    return load_scene(fname);
  }

  IGL_INLINE bool Viewer::load_scene(std::string fname)
  {
   // igl::deserialize(core(),"Core",fname.c_str());
    igl::deserialize(*data(),"Data",fname.c_str());
    return true;
  }

  IGL_INLINE bool Viewer::save_scene()
  {
    std::string fname = igl::file_dialog_save();
    if (fname.length() == 0)
      return false;
    return save_scene(fname);
  }

  IGL_INLINE bool Viewer::save_scene(std::string fname)
  {
    //igl::serialize(core(),"Core",fname.c_str(),true);
    igl::serialize(data(),"Data",fname.c_str());

    return true;
  }

  IGL_INLINE void Viewer::open_dialog_load_mesh()
  {
    std::string fname = igl::file_dialog_open();

    if (fname.length() == 0)
      return;
    
    this->load_mesh_from_file(fname.c_str());
  }

  IGL_INLINE void Viewer::open_dialog_save_mesh()
  {
    std::string fname = igl::file_dialog_save();

    if(fname.length() == 0)
      return;

    this->save_mesh_to_file(fname.c_str());
  }

  IGL_INLINE ViewerData* Viewer::data(int mesh_id /*= -1*/)
  {
    assert(!data_list.empty() && "data_list should never be empty");
    int index;
    if (mesh_id == -1)
      index = selected_data_index;
    else
      index = mesh_index(mesh_id);

    assert((index >= 0 && index < data_list.size()) &&
      "selected_data_index or mesh_id should be in bounds");
    return data_list[index];
  }

  IGL_INLINE const ViewerData* Viewer::data(int mesh_id /*= -1*/) const
  {
    assert(!data_list.empty() && "data_list should never be empty");
    int index;
    if (mesh_id == -1)
      index = selected_data_index;
    else
      index = mesh_index(mesh_id);

    assert((index >= 0 && index < data_list.size()) &&
      "selected_data_index or mesh_id should be in bounds");
    return data_list[index];
  }

  IGL_INLINE int Viewer::append_mesh(bool visible /*= true*/)
  {
    assert(data_list.size() >= 1);

    data_list.emplace_back(new ViewerData());
    selected_data_index = data_list.size()-1;
    data_list.back()->id = next_data_id++;
    //if (visible)
    //    for (int i = 0; i < core_list.size(); i++)
    //        data_list.back().set_visible(true, core_list[i].id);
    //else
    //    data_list.back().is_visible = 0;
    return data_list.back()->id;
  }

  IGL_INLINE bool Viewer::erase_mesh(const size_t index)
  {
    assert((index >= 0 && index < data_list.size()) && "index should be in bounds");
    assert(data_list.size() >= 1);
    if(data_list.size() == 1)
    {
      // Cannot remove last mesh
      return false;
    }
    data_list[index]->meshgl.free();
    data_list.erase(data_list.begin() + index);
    if(selected_data_index >= index && selected_data_index > 0)
    {
      selected_data_index--;
    }

    return true;
  }

  IGL_INLINE size_t Viewer::mesh_index(const int id) const {
    for (size_t i = 0; i < data_list.size(); ++i)
    {
      if (data_list[i]->id == id)
        return i;
    }
    return 0;
  }

  Eigen::Matrix4d Viewer::CalcParentsTrans(int indx) 
  {
	  Eigen::Matrix4d prevTrans = Eigen::Matrix4d::Identity();

	  for (int i = indx; parents[i] >= 0; i = parents[i])
	  {
		  prevTrans = data_list[parents[i]]->MakeTransd() * prevTrans;
	  }

	  return prevTrans;
  }


    IGL_INLINE void Viewer::Draw(int shaderIndx, const Eigen::Matrix4f &Proj, const Eigen::Matrix4f &View, int viewportIndx, unsigned int flgs,unsigned int property_id)
    {

        Eigen::Matrix4f Normal;

        if (!(staticScene & (1<<viewportIndx)))
            Normal = MakeTransScale();
        else
            Normal = Eigen::Matrix4f::Identity();

        for (int i = 0; i < data_list.size(); i++)
        {
            auto shape = data_list[i];
            if (shape->Is2Render(viewportIndx))
            {

                Eigen::Matrix4f Model = shape->MakeTransScale();

                if (!shape->IsStatic())
                {

                    Model = Normal * GetPriviousTrans(View.cast<double>(),i).cast<float>() * Model;
                }
                else if (parents[i] == -2) {
                    Model = View.inverse() * Model;
                }
                if (!(flgs & 65536))
                {
                    Update(Proj, View, Model, shape->GetShader(),i);
                    // Draw fill
                    if (shape->show_faces & property_id)
                        shape->Draw(shaders[shape->GetShader()], true);
                    if (shape->show_lines & property_id) {
                        glLineWidth(shape->line_width);
                        shape->Draw(shaders[shape->GetShader()],false);
                    }
                    // overlay draws
                    if(shape->show_overlay & property_id){
                        if (shape->show_overlay_depth & property_id)
                            glEnable(GL_DEPTH_TEST);
                        else
                            glDisable(GL_DEPTH_TEST);
                        if (shape->lines.rows() > 0)
                        {
                            Update_overlay(Proj, View, Model,i,false);
                            glEnable(GL_LINE_SMOOTH);
                            shape->Draw_overlay(overlay_shader,false);
                        }
                        if (shape->points.rows() > 0)
                        {
                            Update_overlay(Proj, View, Model,i,true);
                            shape->Draw_overlay_pints(overlay_point_shader,false);
                        }
                    glEnable(GL_DEPTH_TEST);
                    }
                }
                else
                { //picking
                    if (flgs & 16384)
                    {
                        Eigen::Affine3f scale_mat = Eigen::Affine3f::Identity();
                        scale_mat.scale(Eigen::Vector3f(1.1f, 1.1f, 1.1f));
                        Update(Proj, View * Normal, Normal.inverse() * Model * scale_mat.matrix(), 0,i);
                    }
                    else
                    {
                        Update(Proj, View * Normal, Normal.inverse() * Model, 0,i);
                    }
                    shape->Draw(shaders[0], true);
                }
            }
        }
    }

    int Viewer::AddShader(const std::string &fileName) {
        shaders.push_back(new Shader(fileName,next_data_id));
        next_data_id +=1;
        return (shaders.size() - 1);
    }

    int Viewer::AddShader(const std::string &Vertex_Shader,const std::string &Fragment_shader ) {
        shaders.push_back(new Shader(Vertex_Shader,Fragment_shader, false,next_data_id));
        next_data_id +=1;
        return (shaders.size() - 1);
    }

    void Viewer::SetShader_overlay(const std::string &fileName) {
         overlay_shader = new Shader(fileName,true,next_data_id);
        next_data_id +=1;
    }

    void Viewer::SetShader_point_overlay(const std::string &fileName) {
        overlay_point_shader = new Shader(fileName,true,next_data_id);
        next_data_id +=1;
    }

    int Viewer::AddShapeFromFile(const std::string& fileName, int parent, unsigned int mode, int viewport)
    {
        this->load_mesh_from_file(fileName);
        data()->type = MeshCopy;
        data()->mode = mode;
        data()->shaderID = 1;
        data()->viewports = 1 << viewport;
        data()->is_visible = true;
        data()->show_lines = 0;
        data()->hide = false;
        data()->show_overlay = 0;
        this->parents.emplace_back(parent);
        return data_list.size() - 1;
    }


    int Viewer::AddShape(int type, int parent, unsigned int mode, int viewport)
    {
      switch(type){
// Axis, Plane, Cube, Octahedron, Tethrahedron, LineCopy, MeshCopy
          case Plane:
              this->load_mesh_from_file("./data/plane.obj");
              break;
          case Cube:
              this->load_mesh_from_file("./data/cube.obj");
              break;
          case Octahedron:
              this->load_mesh_from_file("./data/octahedron.obj");
              break;
          case Tethrahedron:
              this->load_mesh_from_file("./data/Tetrahedron.obj");
              break;
          case Sphere:
              this->load_mesh_from_file("./data/sphere.obj");
              break;
          case Axis:
              this->load_mesh_from_file("./data/cube.obj");
          default:
              break;

      }
      data()->type = type;
      data()->mode = mode;
      data()->shaderID = 1;
      data()->viewports = 1 << viewport;
      data()->is_visible = 0x1;
      data()->show_lines = 0;
      data()->show_overlay = 0;
      data()->hide = false;
      if(type == Axis){
          data()->is_visible = 0;
          data()->show_faces = 0;
          data()->show_lines = 0;
          data()->show_overlay = 0xFF;
          data()->add_edges((Eigen::RowVector3d::UnitX()*4),-(Eigen::RowVector3d::UnitX()*4),Eigen::RowVector3d(255,0,0));
          data()->add_edges((Eigen::RowVector3d::UnitY()*4),-(Eigen::RowVector3d::UnitY()*4),Eigen::RowVector3d(0,255,0));
      }
      this->parents.emplace_back(parent);
      return data_list.size() - 1;
    }



    int Viewer::AddShapeCopy(int indx, int parent, unsigned int mode, int viewport )
    {
        load_mesh_from_data(data_list[indx]->V, data_list[indx]->F,data_list[indx]->V_uv, data_list[indx]->F_uv);
        data()->type = data_list[indx]->type;
        data()->mode = mode;
        data()->shaderID = data_list[indx]->shaderID;
        data()->viewports = 1 << viewport;
        data()->is_visible = true;
        data()->show_lines = 0;
        data()->show_overlay = 0;
        data()->hide = false;
        this->parents.emplace_back(parent);
        return data_list.size() - 1;
    }

    int Viewer::AddShapeFromData(const Eigen::MatrixXd &V,
                                 const Eigen::MatrixXi &F,
                                 const Eigen::MatrixXd &UV_V,
                                 const Eigen::MatrixXi &UV_F
                                 ,int type, int parent, unsigned int mode, int viewport)
    {
        load_mesh_from_data(V, F, UV_V, UV_F);
        data()->type = type;
        data()->mode = mode;
        data()->shaderID = 1;
        data()->viewports = 1 << viewport;
        data()->is_visible = true;
        data()->show_lines = 0;
        data()->show_overlay = 0;
        data()->hide = false;
        this->parents.emplace_back(parent);
        return data_list.size() - 1;
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
    void Viewer::MouseProccessing(int button, int xrel, int yrel, float movCoeff, Eigen::Matrix4d cameraMat,int viewportIndx)
    {
        Eigen::Matrix4d scnMat = Eigen::Matrix4d::Identity();
        if (pickedShape <= 0 && !(staticScene & (1 << viewportIndx)))
            scnMat = MakeTransd().inverse();
        else if(!(staticScene & (1 << viewportIndx)))
            scnMat = (MakeTransd() * GetPriviousTrans(Eigen::Matrix4d::Identity(),pickedShape)).inverse();
        else if(pickedShape > 0)
            scnMat = (GetPriviousTrans(Eigen::Matrix4d::Identity(),pickedShape)).inverse();

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
                    WhenRotate(scnMat * cameraMat, -((float)xrel/180) / movCoeff, ((float)yrel/180) / movCoeff);

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

    void Viewer::ShapeTransformation(int type, float amt,int mode)
    {
        if (abs(amt) > 1e-5 && !data_list[pickedShape]->IsStatic())
        {
            switch (type)
            {
                case xTranslate:
                    data_list[pickedShape]->MyTranslate(Eigen::Vector3d(amt, 0, 0), mode);
                    break;
                case yTranslate:
                    data_list[pickedShape]->MyTranslate(Eigen::Vector3d(0, amt, 0), mode);
                    break;
                case zTranslate:
                    data_list[pickedShape]->MyTranslate(Eigen::Vector3d(0, 0, amt), mode);
                    break;
                case xRotate:
                    data_list[pickedShape]->MyRotate(Eigen::Vector3d(1, 0, 0), amt, mode);
                    break;
                case yRotate:
                    data_list[pickedShape]->MyRotate(Eigen::Vector3d(0, 1, 0), amt, mode);
                    break;
                case zRotate:
                    data_list[pickedShape]->MyRotate(Eigen::Vector3d(0, 0, 1), amt, mode);
                    break;
                case xScale:
                    data_list[pickedShape]->MyScale(Eigen::Vector3d(amt, 1, 1));
                    break;
                case yScale:
                    data_list[pickedShape]->MyScale(Eigen::Vector3d(1, amt, 1));
                    break;
                case zScale:
                    data_list[pickedShape]->MyScale(Eigen::Vector3d(1,1, amt));
                    break;
                case scaleAll:
                    data_list[pickedShape]->MyScale(Eigen::Vector3d(amt, amt, amt));
                    break;
                case reset:
                    data_list[pickedShape]->ZeroTrans();
                    break;
                default:
                    break;
            }
        }

    }

    bool Viewer::Picking(unsigned char data[4], int newViewportIndx)
    {

        return false;

    }

    void Viewer::WhenTranslate( const Eigen::Matrix4d& preMat, float dx, float dy)
    {
        Movable* obj;
        if (pickedShape == -1 || data_list[pickedShape]->IsStatic())
            obj = (Movable*)this;
        else { obj = (Movable *) data_list[pickedShape]; }
        obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Eigen::Vector3d(dx, 0, 0));
        obj->TranslateInSystem(preMat.block<3, 3>(0, 0), Eigen::Vector3d(0, dy, 0));
        WhenTranslate(dx,dy);
    }

    void Viewer::WhenRotate(const Eigen::Matrix4d& preMat, float dx, float dy)
    {
        Movable* obj;
        if (pickedShape == -1 || data_list[pickedShape]->IsStatic() )
            obj = (Movable*)this;
        else
        {
            int ps = pickedShape;
            for (; parents[ps] > -1; ps = parents[ps]);
            obj = (Movable*)data_list[ps];
        }
        obj->RotateInSystem(Eigen::Vector3d(0, 1, 0), dx);
        obj->RotateInSystem( Eigen::Vector3d(1, 0, 0), dy);
        WhenRotate(dx, dy);
    }

    void Viewer::WhenScroll(const Eigen::Matrix4d& preMat, float dy)
    {
        if (pickedShape == -1 || data_list[pickedShape]->IsStatic())
            this->TranslateInSystem(preMat.block<3,3>(0,0), Eigen::Vector3d(0, 0, dy));
        else
            data_list[pickedShape]->TranslateInSystem(preMat.block<3,3>(0,0), Eigen::Vector3d(0, 0, dy));
        WhenScroll(dy);
    }

    int Viewer::AddMaterial(unsigned int texIndices[], unsigned int slots[], unsigned int size)
    {
        materials.push_back(new Material(texIndices, slots, size));
        return (materials.size() - 1);
    }

    Eigen::Matrix4d Viewer::GetPriviousTrans(const Eigen::Matrix4d& View, unsigned int index)
    {
        Eigen::Matrix4d Model = Eigen::Matrix4d::Identity();
        int p = index >= 0 ? parents[index] : -1;
        for (; p >= 0; p = parents[p])
            Model = data_list[p]->MakeTransd() * Model;
        if (p == -2)
            return  View.inverse() * Model;
        else
            return Model;
    }

    float Viewer::AddPickedShapes(const Eigen::Matrix4d& PV, const Eigen::Vector4i& viewport, int viewportIndx, int left, int right, int up, int bottom,int newViewportIndx)
    {
        //not correct when the shape is scaled
        Eigen::Matrix4d MVP = PV * MakeTransd();
        std::cout << "picked shapes  ";
        bool isFound = false;
        for (int i = 1; i < data_list.size(); i++)
        { //add to pShapes if the center in range
            Eigen::Matrix4d Model = data_list[i]->MakeTransd();
            Model = CalcParentsTrans(i) * Model;
            Eigen::Vector4d pos = MVP * Model * Eigen::Vector4d(0,0,0,1);
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
            Eigen::Vector4d tmp = MVP  * GetPriviousTrans(Eigen::Matrix4d::Identity(),pickedShape) * data_list[pickedShape]->MakeTransd() * Eigen::Vector4d(0, 0, 1, 1);
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

    void Viewer::BindMaterial(Shader* s, unsigned int materialIndx)
    {

        for (int i = 0; i < materials[materialIndx]->GetNumOfTexs(); i++)
        {
            materials[materialIndx]->Bind(textures, i);
            s->SetUniform1i("sampler" + std::to_string(i + 1), materials[materialIndx]->GetSlot(i));
        }
    }

    int Viewer::AddTexture(int width, int height, unsigned char* data, int mode)
    {
        textures.push_back(new Texture(width, height));

        switch (mode)
        {
            case COLOR:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
                break;
            case DEPTH:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
                break;
            case STENCIL:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, data);
                break;
            default:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return(textures.size() - 1);
    }

    void Viewer::Update_overlay(const Eigen::Matrix4f &Proj, const Eigen::Matrix4f &View, const Eigen::Matrix4f &Model, unsigned int shapeIndx,bool is_points) {
        auto data = data_list[shapeIndx];
        Shader * s = is_points ? overlay_point_shader : overlay_shader;
        if(s != nullptr) {
            s->Bind();
            s->SetUniformMat4f("Proj", Proj);
            s->SetUniformMat4f("View", View);
            s->SetUniformMat4f("Model", Model);
        }
    }


    void Viewer::SetParent(int indx, int newValue, bool savePosition)
    {
        parents[indx] = newValue;
        if (savePosition)
        {
            Eigen::Vector4d tmp = data_list[newValue]->MakeTransd() * (data_list[indx]->MakeTransd()).inverse() * Eigen::Vector4d(0,0,0,1);
            data_list[indx]->ZeroTrans();
            data_list[indx]->MyTranslate(-tmp.head<3>(), false);
        }
    }


} // end namespace
} // end namespace
}
