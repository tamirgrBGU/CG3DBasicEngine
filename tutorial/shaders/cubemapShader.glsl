#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;
in vec3 lookat; // direction vector representing a 3D texture coordinate

uniform vec4 lightColor;
uniform samplerCube sampler1;
uniform vec4 lightDirection;

out vec4 Color;
void main()
{
    Color = texture(sampler1, lookat);
}  


