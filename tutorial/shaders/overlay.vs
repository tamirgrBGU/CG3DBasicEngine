#version 330

attribute vec3 position;
attribute vec3 color;
out vec3 color_frag;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

  void main()
  {
    color_frag = color;
    gl_Position = Proj * View * Model * vec4 (position, 1.0);
  }