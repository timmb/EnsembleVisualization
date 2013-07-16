#version 120

uniform sampler2D Tex;
varying vec2 Uv;

void main()
{
	gl_FragColor = texture2D(Uv);
}
