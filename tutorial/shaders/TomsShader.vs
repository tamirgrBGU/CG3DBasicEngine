#version 330

attribute vec2 texcoord;
attribute vec4 position;

out vec2 texcoord0;

void main()
{
    texcoord0 = texcoord;
    gl_Position = position;

}