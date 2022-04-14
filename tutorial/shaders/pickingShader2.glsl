#version 330


uniform sampler2D sampler1;
uniform vec4 lightColor;

in vec3 normal0;
out vec4 Color;
void main()
{
	Color = vec4(lightColor.rgb * abs(normal0),lightColor.a);
}
