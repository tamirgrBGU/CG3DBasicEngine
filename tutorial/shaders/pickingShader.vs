#version 330

attribute vec3 position;
attribute vec3 normal;
attribute vec3 color;
attribute vec4 Ka;
attribute vec4 Kd;
attribute vec4 Ks;
attribute vec2 texCoords;


uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

//out vec3 color0;

void main()
{
	//color0 = color;
	gl_Position = Proj *View * Model* vec4(position, 1.0);
}
