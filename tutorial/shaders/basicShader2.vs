#version 330

attribute vec3 position;
attribute vec3 normal;
attribute vec4 Ka;
attribute vec4 Kd;
attribute vec4 Ks;
attribute vec2 texcoord;

out vec2 texCoords0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

void main()
{

	texCoords0 = texcoord;
	color0 = vec3(Kd);
	normal0 = (Model  * vec4(normal, 0.0)).xyz;
	position0 = vec3(Model * vec4(position, 1.0));
	gl_Position = Proj *View * Model* vec4(position, 1.0); //you must have gl_Position
}
