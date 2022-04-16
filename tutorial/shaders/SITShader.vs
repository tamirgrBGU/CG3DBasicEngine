#version 330

attribute vec2 texcoord;
attribute vec4 position;

out vec2 texcoord0;
out vec4 position0;

void main()
{
    texcoord0 = texcoord;
    position0 = position;
    gl_Position = position;

}