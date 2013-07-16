#version 120

//varying vec2 Uv;
uniform sampler2D Rand;

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0, 1);
	gl_Position.z = 0;
//	Uv = gl_MultiTexCoord0.st;
	gl_PointSize = gl_Vertex.w*1000+10;
}