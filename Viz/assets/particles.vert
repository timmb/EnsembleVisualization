#version 120

varying vec2 Uv;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	Uv = gl_MultiTexCoord0.st;
}