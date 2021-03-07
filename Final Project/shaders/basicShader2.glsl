#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform vec4 lightDirection;

uniform vec4 eyePosition;
uniform float x;

float calcdiffuse(vec3 N, vec3 D){
	return max(dot(N, normalize(D)), 0.0);
}

float calcspecular(vec3 N, vec3 D,  vec3 viewDir, float n){
	vec3 reflectDir = reflect(-normalize(D), N);
	float spec = max(dot(viewDir, reflectDir), 0.0);
	return 0.7 * pow(spec, n);
}

vec3 colorcalc(vec3 objectColor){
	vec3 viewdir = normalize( eyePosition.xyz - vec3(position0.xy, 0) );
	vec3 lightDir = vec3(0.5, 1, 1.0);
	int specpower = 20;
	float kd = calcdiffuse(normalize(normal0), normalize(lightDir));
	float ks = calcspecular(normalize(normal0), normalize(lightDir), viewdir, specpower);
	vec3 ambient = vec3(0.3, 0.1, 0.6);
	vec3 retcolor = ambient + (kd + ks) * objectColor;
	return clamp(retcolor, 0, 1);
}

void main()
{
	gl_FragColor =  ( texture2D(sampler1, texCoord0)*0.6 + texture2D(sampler2, texCoord0)*0.4 )* vec4(colorcalc(color0),1.0); //you must have gl_FragColor
}
