#version 120

//varying vec2 Uv;
uniform sampler2D Rand;
varying float brightness;

int randomCount = -1;
float id;
float amount;


float sq(float x)
{
	return x*x;
}

float rand()
{
	randomCount = randomCount + 1;
	return texture2D(Rand, vec2(id, randomCount)).x;
}

void main()
{
	id = gl_Vertex.z;
	amount = gl_Vertex.w;
	gl_Position = vec4(gl_Vertex.xy, 0, 1);
	gl_Position.z = 0;
//	Uv = gl_MultiTexCoord0.st;
	gl_PointSize = gl_Vertex.w*400+6;
	brightness = rand() ;//sq(rand()*0.63) * (0.3+0.7*amount);
}
