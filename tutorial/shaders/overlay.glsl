#version 330
in vec3 color_frag;
out vec4 outColor;

void main()
  {
    outColor = vec4(color_frag, 1.0);
  }