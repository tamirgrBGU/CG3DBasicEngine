#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;
uniform float time;
uniform float x;
uniform float y;

out vec4 Color;
void main()
{
	float angle = time;
	vec3 lightPos = vec3(0,0,1);
		
	vec2 tmp = vec2(abs(cos(angle)*texCoord0.s - sin(angle)*texCoord0.t), abs(sin(angle)*texCoord0.s + cos(angle)*texCoord0.t));
	if((x-texCoord0.s)*(x-texCoord0.s) + (1-y - texCoord0.t)*(1-y-texCoord0.t) < 0.01)
	{
		vec3 normal1 = vec3(texCoord0.s-x,texCoord0.t - (1-y),sqrt(0.01 - (texCoord0.s-x)*(texCoord0.s-x) - (texCoord0.t - (1-y))*(texCoord0.t - (1-y))));
		vec3 lightD = vec3(texCoord0.s,texCoord0.t,0) - lightPos;

		Color = vec4(1,1,1,1)*dot(normalize(normal1),-normalize(lightD));
	}
	else
		Color = texture(sampler1, tmp)* lightColor; //you must have gl_FragColor
}
