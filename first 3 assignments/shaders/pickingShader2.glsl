#version 330

in vec3 normal0;

//uniform sampler2D sampler1;
//uniform vec4 lightDirection;
uniform vec4 lightColor;

void main()
{
	gl_FragColor = vec4(lightColor.r, abs(normal0)); //vec4(lightColor.rgb * abs(normal0),lightColor.a);
}