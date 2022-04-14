#version 330
in vec3 color_frag;
out vec4 outColor;
void main()
  {
  if (length(gl_PointCoord - vec2(0.5)) > 0.5)
    discard;
  outColor = vec4(color_frag, 1.0);
  }