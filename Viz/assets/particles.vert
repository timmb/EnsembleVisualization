#version 120

//varying vec2 Uv;
uniform sampler2D Rand;
varying float brightness;
uniform int numRandomsPerParticle;
uniform float time;

int randomCount = -1;
float id;
// lookup in random texture, which is organised into columns (see Renderer.cpp)
float randRow;
float randColOffset;
float amount;

const float pi = 3.14159;


float sq(float x)
{
	return x*x;
}

float rand()
{
	randomCount = randomCount + 1;
	return texture2D(Rand, vec2(randRow, randColOffset + randomCount)).x;
}




//
// Description : Array and textureless GLSL 2D/3D/4D simplex
// noise functions.
// Author : Ian McEwan, Ashima Arts.
// Maintainer : ijm
// Lastmod : 20110822 (ijm)
// License : Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise
//

vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
	return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{
	const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);
	
	// First corner
	vec3 i = floor(v + dot(v, C.yyy) );
	vec3 x0 = v - i + dot(i, C.xxx) ;
	
	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );
	
	// x0 = x0 - 0.0 + 0.0 * C.xxx;
	// x1 = x0 - i1 + 1.0 * C.xxx;
	// x2 = x0 - i2 + 2.0 * C.xxx;
	// x3 = x0 - 1.0 + 3.0 * C.xxx;
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y
	
	// Permutations
	i = mod289(i);
	vec4 p = permute( permute( permute(
									   i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
							  + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
					 + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
	
	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3 ns = n_ * D.wyz - D.xzx;
	
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)
	
	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ ); // mod(j,N)
	
	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);
	
	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );
	
	//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
	//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));
	
	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
	
	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);
	
	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	
	// Mix final noise value
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
								 dot(p2,x2), dot(p3,x3) ) );
}

// -------------------------------------------------------


vec4 calculatePositionNoise()
{
	float noise = snoise(vec3(gl_Vertex.xy, time*(rand()*.16+.1)*.4));
	noise *= noise;
	float noise2 = snoise(vec3(gl_Vertex.xy, time*(rand()*.6+.1)))*(sin(time)+0.5);
	float noise3 = snoise(vec3(gl_Vertex.xy, time*0.007));
	return vec4(cos(2*pi*noise+noise3), sin(2*pi*noise+noise3), 0, 0)*(0.015+(noise2*0.02-0.05) + 0.03*noise3);
}


void main()
{
	//tmp
//	gl_Position = vec4(0);
//	return;
	id = gl_Vertex.z;
	randRow = mod(id, 10000);
	randColOffset = (randRow / 10000) * numRandomsPerParticle;
	amount = gl_Vertex.w;
	gl_Position = vec4(gl_Vertex.xy, 0, 1);
	gl_Position += calculatePositionNoise();
	gl_Position.z = 0;
	//	Uv = gl_MultiTexCoord0.st;
	gl_PointSize = 20.12*(1+2*cos(rand())-0.5)*0.7*amount + 6;
	brightness = rand()*.1315 ;//sq(rand()*0.63) * (0.3+0.7*amount);
}


