#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;


void main()
{
//gl_FragColor = vec4(1,0,0,1);
	gl_FragColor = texture2D(sampler1, texCoord0); //you must have gl_FragColor
}
