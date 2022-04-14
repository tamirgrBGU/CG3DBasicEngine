#version 330

uniform sampler1D tex;
uniform vec4 data;
uniform int iter;
uniform float p;


in vec2 texCoords0;
out vec4 Color;
void main() {
	vec2 z, c;

	c.x = (texCoords0.x - 0.5) * data.z - data.x;
	c.y = (texCoords0.y - 0.5) * data.z - data.y;

	int i;
	z = c;
	for(i=0; i<iter; i++) {
        float r = sqrt(z.x * z.x + z.y * z.y), angle = atan(z.y,z.x);     
        float sn = sin(angle*p);
        float cn = cos(angle*p);
		float x = cn*pow(r,p) + c.x;
		float y = sn*pow(r,p) + c.y;
       // float x =  (z.x * z.x - z.y * z.y) + c.x;
		//float y = (z.y * z.x + z.x * z.y) + c.y;
		if((x * x + y * y) > 4.0) break;
		z.x = x;
		z.y = y;
	}

	Color = texture(tex, (i == iter ? 0.0 : float(i)) / data.w*128.0f/iter);
}
