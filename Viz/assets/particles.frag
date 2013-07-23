#version 120

uniform sampler2D Tex;
varying float brightness;

void main()
{
	gl_FragColor = texture2D(Tex, gl_PointCoord);
	gl_FragColor.a *= .15*brightness;
}
