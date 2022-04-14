#version 330

//varying vec3 color0;

uniform sampler2D sampler1;
uniform vec4 lightDirection;
uniform vec4 lightColor;
out vec4 Color;
void main()
{
	Color = lightColor;
}
