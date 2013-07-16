//
//  Renderer.cpp
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "Renderer.h"
#include <map>
#include <algorithm>
#include "cinder/Surface.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace std;

void Renderer::setState(State const& newState)
{
	mState = newState;
}

State Renderer::state() const
{
	return mState;
}

Renderer::Renderer()
: mEnableDrawConnectionsDebug(false)
{
	Surface blob = Surface(loadImage(app::getAssetPath("blob.png")));
	mParticleTex = gl::Texture::create(blob);
	
//	ofImage blob;
//	blob.loadImage("blob.png");
//	mParticleTex.allocate(blob.width, blob.height, GL_RGBA8);
//	mParticleTex.loadData(blob);

	for (int i=0; i<NUM_INSTRUMENTS; ++i)
		for (int j=0; j<NUM_INSTRUMENTS; ++j)
			mControlPoints[i][j] = vector<ci::Vec2f>();
	updateCalculatedControlPoints();
}

void Renderer::setEnableDrawConnectionsDebug(bool enabled)
{
	mEnableDrawConnectionsDebug = enabled;
}

Renderer::~Renderer()
{
	
}

void Renderer::draw(float elapsedTime, float dt)
{
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// normalized coordinates: 2x2 square centred at origin
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mParticleTex->getId());


	
//	glMatrixMode(GL_TEXTURE);
//	// \TODO: wtf is this push for? it's unmatched?
//	glPushMatrix();
//	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	int N = 6000;
//	N = 100;
	map<int,int> pCount;
	map<int,int> qCount;
	for (int i=0; i<N; ++i)
	{
//		if (i!=76)
//			continue;
		int p = i%NUM_INSTRUMENTS;
		int q = (i/NUM_INSTRUMENTS)%NUM_INSTRUMENTS; // dest
		if (p<=q)
			continue;
		Rand random;
		random.seed(i*12421232);
//		ofSeedRandom(i*12421232);
		float period = random.nextFloat(50.)+10;
		float phase = random.nextFloat(1.)*period;
		float t = fmod((elapsedTime+phase)/period, 1.f);
		t *= min(1.f, t+0.2f);
//		Vec2f const& orig = mState.instruments.at(p).pos;
//		Vec2f const& dest = mState.instruments.at(q).pos;
		float amount = mState.instruments.at(p).connections.at(q);
		if (0)//(amount>0.3 && ofGetFrameNum()%30==0)
		{
			printf("Points for instrument %s and %s:\n", mState.instruments[p].name.c_str(), mState.instruments[q].name.c_str());
			cout << mControlPoints[p][q];
			cout << endl << endl;
		}
//		printf("p %d q %d amount %f\n", p, q, amount);
//		if (amount<0.5)
//			continue;
		Vec2f point;
		point = interpHermite(p, q, t);
		random.seed((NUM_INSTRUMENTS*p + q)*123232);
		float noise = mPerlin.noise(point.x, point.y, elapsedTime*(random.nextFloat(0.1,0.7))*.4);
		float noise2 = mPerlin.noise(point.x, point.y, elapsedTime*(random.nextFloat(0.1, 0.7)))*(sin(elapsedTime)+0.5f);
		float noise3 = mPerlin.noise(point.x, point.y, elapsedTime*0.007);
		noise *= noise*noise;
		random.seed(i*124212);
		float brightness = sq(random.nextFloat(1)*0.63) * (0.3+0.7*amount);
		float scaleFactor = 1.+2*cos(random.nextFloat(-0.5, 0.5)) * 0.7;
		float size = 0.012f *scaleFactor * amount;
		point += Vec2f(cos(2*PI*noise+noise3), sin(2*PI*noise+noise3)) * (0.015+(noise2*0.02-0.05) + 0.03*noise3);
		float redness = 0.f;
		if (random.nextFloat(1)>0.94)
			redness = random.nextFloat(0.3, 0.7);
		glColor4f(0.94,1-redness*redness,1-redness, brightness);
//		gl::draw(mParticleTex, Rectf(point, Vec2f(size, size)));
		gl::color(ColorAf(1,1,1,1));
		mParticleTex->bind();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mParticleTex->getId());
//		size = 0.1;
		gl::drawSolidRect(Rectf(point-Vec2f(size, size), point+Vec2f(size, size)));

	}
	glBindTexture(GL_TEXTURE_2D, NULL);
	
	if (mEnableDrawConnectionsDebug)
		drawConnectionsDebug();
}

void Renderer::setControlPoints(ControlPointMap const& points)
{
	mControlPoints = points;
	updateCalculatedControlPoints();
}

ControlPointMap Renderer::controlPoints() const
{
	return mControlPoints;
}


struct VecComparer
{
	bool operator()(Vec2f const* a, Vec2f const* b)
	{
		return mDistSq[a] > mDistSq[b];
	}
	
	static map<Vec2f const*, float> mDistSq;
};

map<Vec2f const*, float> VecComparer::mDistSq;

bool compareVec2fLex(Vec2f const& a, Vec2f const& b)
{
	return
	  a.x < b.x? true
	: a.x > b.x? false
	: a.y < b.y? true
	: a.y > b.y? false
	           : false;
}

int fact(int t)
{
	int x = 1;
	for (int i=1; i<=t; ++i)
	{
		x *= i;
	}
	return x;
}

float calculateDistSqOfPath(vector<Vec2f> const& points)
{
	float distSq = 0.f;
	for (int l=1; l<points.size(); ++l)
	{
		distSq += points[l-1].distanceSquared(points[l]);
	}
	return distSq;
}

//
//ofVec2f Renderer::interp(int inst0, int inst1, float t)
//{
//	std::vector<ofVec2f> const& bezierPoints = mControlPoints[inst0][inst1];
//	// we need 4 points for a bezier segment
//	const int numSegments = bezierPoints.size() - 3;
//	int segment = min(numSegments-1, int(t*numSegments));
//	float p = t*numSegments - segment;
//	assert(0<=p && p<=1);
//	assert(segment < bezierPoints.size()-3);
//	return bezierInterp(bezierPoints[segment], bezierPoints[segment+1], bezierPoints[segment+2], bezierPoints[segment+3], p);
//
//}
//
//ofVec2f Renderer::interp(ofVec2f const& orig, ofVec2f const& dest, float t)
//{
//	priority_queue<ofVec2f const*, vector<ofVec2f const*>, VecComparer> queue;
//	for (ofVec2f const& v: mPoints)
//	{
//		float d = orig.distanceSquared(v);
//		VecComparer::mDistSq[&v] = d;
//		queue.push(&v);
//	}
//	
//	const int NUM_CONTROL_POINTS = min<int>(4, mPoints.size());
//	// plus start and end
//	std::vector<ofVec2f const*> bezierPoints;
//	bezierPoints.reserve(NUM_CONTROL_POINTS+2);
//	bezierPoints.push_back(&orig);
//	for (int i=0; i<NUM_CONTROL_POINTS; ++i)
//	{
//		bezierPoints.push_back(queue.top());
//		queue.pop();
//	}
//	bezierPoints.push_back(&dest);
//	if (bezierPoints.size() < 4) // in case we have no control points
//	{
//		bezierPoints.push_back(&orig);
//		bezierPoints.push_back(&dest);
//	}
//	// we need 4 points for a bezier segment
//	const int numSegments = bezierPoints.size() - 3;
//	int segment = min(numSegments-1, int(t*numSegments));
//	float p = t*numSegments - segment;
//	assert(0<=p && p<=1);
//	assert(segment < bezierPoints.size()-3);
//	return bezierInterp(*bezierPoints[segment], *bezierPoints[segment+1], *bezierPoints[segment+2], *bezierPoints[segment+3], p);
//	
//
//}

void Renderer::drawQuad(Vec2f const& pos, Vec2f const& size)
{
	glPushMatrix();
	gl::translate(pos);
	glScalef(size.x, size.y, 1);
	
	
	
	static GLfloat vertices[] = {
		-1., -1.,
		-1.,  1.,
		 1.,  1.,
		 1., -1.
	};
	static GLfloat uvs[] = {
		0., 0.,
		0., 1.,
		1., 1.,
		1., 0.
	};
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, uvs);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void Renderer::drawConnectionsDebug()
{
	auto& instruments = mState.instruments;
	for (int j=0; j<instruments.size(); ++j)
	{
		Instrument inst = instruments[j];
		for (int i=0; i<instruments.size(); ++i)
		{
			float f = 15*inst.connections.at(i);
			glColor4f(1,1,1,min(1.f,f)*0.5);
			gl::lineWidth(f);
			gl::drawLine(inst.pos, instruments.at(i).pos);
		}
	}
	gl::lineWidth(1.f);
}

template <typename T>
T Renderer::hermiteSpline(T const& point0, T const& tangent0, T const& point1, T const& tangent1, float s)
{
	float h1 =  2*s*s*s - 3*s*s + 1;          // calculate basis function 1
	float h2 = -2*s*s*s + 3*s*s;              // calculate basis function 2
	float h3 =   s*s*s  - 2*s*s + s;         // calculate basis function 3
	float h4 =   s*s*s  -  s*s;              // calculate basis function 4
	T p = h1*point0 +                    // multiply and sum all funtions
	h2*point1 +                    // together to build the interpolated
	h3*tangent0 +                    // point along the curve.
	h4*tangent1;
	return p;
}

template <typename T>
T Renderer::hermiteSpline(vector<T> const& points, float t)
{
	assert(points.size()>=2);
	const int numSegments = points.size() - 1;
	int segment = min(numSegments-1, int(t*numSegments));
	float p = t*numSegments - segment;
	assert(0<=p && p<=1);
	assert(segment < points.size()-1);
	// last segment is a special case
	// ...
	T tangent0 = points[segment+1]-points[segment];
	T tangent1 =
		segment+2>=points.size()? T()
								: points[segment+2]-points[segment];
	return hermiteSpline(points[segment], tangent0, points[segment+1], tangent1, p);
	
}

Vec2f Renderer::interpHermite(int inst0, int inst1, float t) const
{
	vector<Vec2f> const& points =mCalculatedControlPoints.at(inst0).at(inst1);
	return hermiteSpline(points, t);
}

void Renderer::updateCalculatedControlPoints()
{
	for (int i=0; i<NUM_INSTRUMENTS; ++i)
		for (int j=0; j<NUM_INSTRUMENTS; ++j)
		{
			auto const& points = mControlPoints[i][j];
			mCalculatedControlPoints[i][j] = vector<Vec2f>(points.size()+2);
			auto& ps = mCalculatedControlPoints[i][j];
			ps[0] = mState.instruments.at(i).pos;
			ps[ps.size()-1] = mState.instruments.at(j).pos;
			copy(points.begin(), points.end(), ps.begin()+1);
			cout << "Control points for "<<i<<" to "<<j<<": "<<points <<endl;
			cout << "Calculated control points for "<<i<<" to "<<j<<": "<<ps <<endl;
		}
}
