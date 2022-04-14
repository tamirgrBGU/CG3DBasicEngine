#version 330

uniform sampler1D tex;
uniform vec4 data;
uniform int iter;

in vec2 texCoords0;

void main() {
	vec2 z, c;

	c.x = 1.3333 * (texCoords0.x - 0.5) * scale - center.x;
	c.y = (texCoords0.y - 0.5) * scale - center.y;

	int i;
	z = c;
	for(i=0; i<iter; i++) {
		float x = (z.x * z.x - z.y * z.y) + c.x;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 4.0) break;
		z.x = x;
		z.y = y;
	}

	gl_FragColor = texture1D(tex, (i == iter ? 0.0 : float(i)) / 100.0);
}
