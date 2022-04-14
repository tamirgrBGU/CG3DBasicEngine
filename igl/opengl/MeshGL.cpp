// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#include "MeshGL.h"
//#include "gl.h"
#include "bind_vertex_attrib_array.h"
//#include "create_shader_program.h"
//#include "destroy_shader_program.h"
#include "verasansmono_compressed.h"
#include <iostream>

IGL_INLINE igl::opengl::MeshGL::MeshGL():
  tex_filter(GL_LINEAR),
  tex_wrap(GL_REPEAT)

{
}

IGL_INLINE void igl::opengl::MeshGL::init_buffers()
{
  // Mesh: Vertex Array Object & Buffer objects
  vao_mesh.Bind();
  vbo_V.Init(nullptr,0,true);
  vbo_V_normals.Init(nullptr,0,true);
  vbo_V_ambient.Init(nullptr,0,true);
  vbo_V_diffuse.Init(nullptr,0,true);
  vbo_V_specular.Init(nullptr,0,true);
  vbo_V_uv.Init(nullptr,0,true);
  vbo_F.Init(nullptr,0);
//  glGenTextures(1, &vbo_tex);
  glGenTextures(1, &font_atlas); // Fonts

  // Line overlay
  vao_overlay_lines.Bind();
  vbo_lines_F.Init(nullptr,0);
  vbo_lines_V.Init(nullptr,0,true);
  vbo_lines_V_colors.Init(nullptr,0,true);

  // Point overlay
  vao_overlay_points.Bind();
  vbo_points_F.Init(nullptr,0);
  vbo_points_V.Init(nullptr,0,true);
  vbo_points_V_colors.Init(nullptr,0,true);

  // Text Labels
  vertex_labels.init_buffers();
  face_labels.init_buffers();
  custom_labels.init_buffers();

  dirty = MeshGL::DIRTY_ALL;
  dirty_mesh_shader = MeshGL::DIRTY_ALL;
}

IGL_INLINE void igl::opengl::MeshGL::free_buffers()
{
  if (is_initialized)
  {
    // Text Labels
    vertex_labels.free_buffers();
    face_labels.free_buffers();
    custom_labels.free_buffers();

    glDeleteTextures(1, &font_atlas);
  }
}

IGL_INLINE void igl::opengl::MeshGL::TextGL::init_buffers()
{
  glGenVertexArrays(1, &vao_labels);
  glBindVertexArray(vao_labels);
  glGenBuffers(1, &vbo_labels_pos);
  glGenBuffers(1, &vbo_labels_characters);
  glGenBuffers(1, &vbo_labels_offset);
  glGenBuffers(1, &vbo_labels_indices);
}

IGL_INLINE void igl::opengl::MeshGL::TextGL::free_buffers()
{
  glDeleteBuffers(1, &vbo_labels_pos);
  glDeleteBuffers(1, &vbo_labels_characters);
  glDeleteBuffers(1, &vbo_labels_offset);
  glDeleteBuffers(1, &vbo_labels_indices);
}

IGL_INLINE void igl::opengl::MeshGL::bind_mesh(unsigned int shader_mesh, unsigned int shader_id)
{
  vao_mesh.Bind();
//  glUseProgram(shader_mesh); Bind out side
  if(dirty_mesh_shader &= ~(1 << shader_id)){
      dirty |= MeshGL::DIRTY_MESH;
  }
  bind_vertex_attrib_array(shader_mesh,"position", vbo_V, V_vbo, dirty & MeshGL::DIRTY_POSITION);
  bind_vertex_attrib_array(shader_mesh,"normal", vbo_V_normals, V_normals_vbo, dirty & MeshGL::DIRTY_NORMAL);
  bind_vertex_attrib_array(shader_mesh,"Ka", vbo_V_ambient, V_ambient_vbo, dirty & MeshGL::DIRTY_AMBIENT);
  bind_vertex_attrib_array(shader_mesh,"Kd", vbo_V_diffuse, V_diffuse_vbo, dirty & MeshGL::DIRTY_DIFFUSE);
  bind_vertex_attrib_array(shader_mesh,"Ks", vbo_V_specular, V_specular_vbo, dirty & MeshGL::DIRTY_SPECULAR);
  bind_vertex_attrib_array(shader_mesh,"texcoord", vbo_V_uv, V_uv_vbo, dirty & MeshGL::DIRTY_UV);

  vbo_F.Bind();
  if (dirty & MeshGL::DIRTY_FACE)
    vbo_F.ChageData(F_vbo.data(),F_vbo.size());
  if (dirty_mesh_shader & (1 << shader_id))
      dirty_mesh_shader &= ~(1 << shader_id);
  else
    dirty &= ~MeshGL::DIRTY_MESH;
}

IGL_INLINE void igl::opengl::MeshGL::bind_overlay_lines(unsigned int shader_overlay_lines,unsigned int shader_id)
{
  bool is_dirty = dirty & MeshGL::DIRTY_OVERLAY_LINES;
  vao_overlay_lines.Bind();
  glUseProgram(shader_overlay_lines);
 bind_vertex_attrib_array(shader_overlay_lines,"position", vbo_lines_V, lines_V_vbo, is_dirty);
 bind_vertex_attrib_array(shader_overlay_lines,"color", vbo_lines_V_colors, lines_V_colors_vbo, is_dirty);

  vbo_lines_F.Bind();
  if (is_dirty)
    vbo_lines_F.ChageData(lines_F_vbo.data(),lines_F_vbo.size());
  if (dirty_mesh_shader & (1 << shader_id))
    dirty_mesh_shader &= ~(1 << shader_id);
  else
    dirty &= ~MeshGL::DIRTY_OVERLAY_LINES;
}

IGL_INLINE void igl::opengl::MeshGL::bind_overlay_points(unsigned int shader_overlay_points,unsigned int shader_id)
{
  bool is_dirty = dirty & MeshGL::DIRTY_OVERLAY_POINTS;
    vao_overlay_points.Bind();
  glUseProgram(shader_overlay_points);
 bind_vertex_attrib_array(shader_overlay_points,"position", vbo_points_V, points_V_vbo, is_dirty);
 bind_vertex_attrib_array(shader_overlay_points,"color", vbo_points_V_colors, points_V_colors_vbo, is_dirty);

 vbo_points_F.Bind();
  if (is_dirty)
      vbo_points_F.ChageData(points_F_vbo.data(),points_F_vbo.size());
  if (dirty_mesh_shader & (1 << shader_id))
    dirty_mesh_shader &= ~(1 << shader_id);
  else
    dirty &= ~MeshGL::DIRTY_OVERLAY_POINTS;
}

IGL_INLINE void igl::opengl::MeshGL::init_text_rendering()
{
  // Decompress the png of the font atlas
  unsigned char verasansmono_font_atlas[256*256];
  decompress_verasansmono_atlas(verasansmono_font_atlas);

  // Bind atlas
  glBindTexture(GL_TEXTURE_2D, font_atlas);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 256, 256, 0, GL_RED, GL_UNSIGNED_BYTE, verasansmono_font_atlas);

  // TextGL initialization
  vertex_labels.dirty_flag = MeshGL::DIRTY_VERTEX_LABELS;
  face_labels.dirty_flag = MeshGL::DIRTY_FACE_LABELS;
  custom_labels.dirty_flag = MeshGL::DIRTY_CUSTOM_LABELS;
}

IGL_INLINE void igl::opengl::MeshGL::bind_labels(const TextGL& labels,unsigned int shader_text)
{
  bool is_dirty = dirty & labels.dirty_flag;
  glBindTexture(GL_TEXTURE_2D, font_atlas);
  glBindVertexArray(labels.vao_labels);
  glUseProgram(shader_text);
  bind_vertex_attrib_array(shader_text, "position" , labels.vbo_labels_pos       , labels.label_pos_vbo   , is_dirty);
  bind_vertex_attrib_array(shader_text, "character", labels.vbo_labels_characters, labels.label_char_vbo  , is_dirty);
  bind_vertex_attrib_array(shader_text, "offset"   , labels.vbo_labels_offset    , labels.label_offset_vbo, is_dirty);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, labels.vbo_labels_indices);
  if (is_dirty)
  {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*labels.label_indices_vbo.size(), labels.label_indices_vbo.data(), GL_DYNAMIC_DRAW);
  }
  dirty &= ~labels.dirty_flag;
}

IGL_INLINE void igl::opengl::MeshGL::draw_mesh(bool solid, unsigned int mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, solid ? GL_FILL : GL_LINE);

  /* Avoid Z-buffer fighting between filled triangles & wireframe lines */
  if (solid)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
  }
  glDrawElements(mode, 3*F_vbo.rows(), GL_UNSIGNED_INT, 0);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
IGL_INLINE void igl::opengl::MeshGL::draw_overlay_lines()
{
  glDrawElements(GL_LINES, lines_F_vbo.rows(), GL_UNSIGNED_INT, 0);
}
IGL_INLINE void igl::opengl::MeshGL::draw_overlay_points()
{
  glDrawElements(GL_POINTS, points_F_vbo.rows(), GL_UNSIGNED_INT, 0);
}
IGL_INLINE void igl::opengl::MeshGL::draw_labels(const TextGL& labels)
{
  glDrawElements(GL_POINTS, labels.label_indices_vbo.rows(), GL_UNSIGNED_INT, 0);
}
IGL_INLINE void igl::opengl::MeshGL::init()
{
  if(is_initialized)
  {
    return;
  }
  is_initialized = true;
  init_buffers();
  init_text_rendering();
}
IGL_INLINE void igl::opengl::MeshGL::free()
{
  if (is_initialized)
  {
    free_buffers();
  }
}
