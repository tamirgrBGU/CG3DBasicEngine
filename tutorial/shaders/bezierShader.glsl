#version 330

#define COEFF_SIZE 20
#define epsilon 0.1
#define IN_RANGE(x0,x1) (x0<x1+epsilon && x0>x1-epsilon)
in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4[COEFF_SIZE] coeffs;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform vec4 lightDirection;

out vec4 Color;

vec3 bezier(vec3 A, vec3 B, vec3 C, vec3 D, float t) {
  vec3 E = mix(A, B, t);
  vec3 F = mix(B, C, t);
  vec3 G = mix(C, D, t);

  vec3 H = mix(E, F, t);
  vec3 I = mix(F, G, t);

  vec3 P = mix(H, I, t);

  return P;
}

float f(float x, int idx) {
    return dot(coeffs[idx] , vec4(x*x*x, x*x, x, 1));
}


void main()
{
    if(IN_RANGE(f(position0.x) , position0.y))
	    Color = vec4(1,0,0,1) // texture2D(sampler1, texCoord0)* vec4(color0,1.0); //you must have gl_FragColor
    else 
        Color = vec4(0,0,0,1)
}
