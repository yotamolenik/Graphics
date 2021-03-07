#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler1D sampler1;
uniform vec4 lightDirection;

uniform float pixelWidth;
uniform float z;
uniform float p;
uniform float x;
uniform float y;
uniform float xOffset;
uniform float yOffset;

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

void main()
{
	float iter = 100;

	float cx = mapRange(texCoord0.x, 1, -pixelWidth + xOffset, pixelWidth + xOffset);
	float cy = mapRange(texCoord0.y, 1, -pixelWidth + yOffset, pixelWidth + yOffset);
	int counter = 0;
	float zx = 0;
	float zy = 0;
	while (counter < iter && zx*zx + zy*zy < 4.0) {
		float c = zx * zx + zy * zy;
		float t = pow(c, p/2) * cos(p * atan(zy,zx)) + cx;
		zy = pow(c, p / 2) * sin(p * atan(zy, zx)) + cy;
		zx = t;
		counter++;
	}

	float color = round(mapRange(counter, iter, 0, z));
	color = mapRange(color, z, 0, 1);
	if (counter == iter) {
		color = 0;
	}

	if(pow(texCoord0.x-x, 2) + pow(texCoord0.y-y, 2) < 0.001){
		gl_FragColor = vec4(0,1,1,1);
	}
	else{
		gl_FragColor = vec4(0, color, color, color);
		gl_FragColor = texture(sampler1, color);
	}
}