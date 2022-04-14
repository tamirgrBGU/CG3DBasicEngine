#version 330

//uniform vec4 lightColor;
uniform sampler2D sampler;

in vec2 texCoords0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;



out vec4 Color;
void main()
{
	Color = texture2D(sampler, texCoords0) * vec4(color0,1.0); //you must have gl_FragColor
}
