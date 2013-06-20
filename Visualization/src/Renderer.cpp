//
//  Renderer.cpp
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "Renderer.h"
#include "ofMain.h"
#include <map>
#include <algorithm>

void Renderer::setState(State const& newState)
{
	mState = newState;
}

Renderer::Renderer()
: hermite(true)
{
	ofImage blob;
	blob.loadImage("blob.png");
	mParticleTex.allocate(blob.width, blob.height, GL_RGBA8);
	mParticleTex.loadData(blob);

	
	// try also:
//Pointsvector: [
//			   -0.085, 0.27
//			   -0.52, -0.21
//			   0.043, -0.69
//			   0.66, -0.37
//			   -0.81, 0.44
//			   ]
	
	mPoints.push_back(ofVec2f(-0.1, 0.054));
	mPoints.push_back(ofVec2f(-0.5, -0.34));
	mPoints.push_back(ofVec2f(0.12, -0.84));
	mPoints.push_back(ofVec2f(0.57, -0.69));
	mPoints.push_back(ofVec2f(-0.15, 0.77));
	mPoints.push_back(ofVec2f(0.48, 0.82));
	mPoints.push_back(ofVec2f(-0.95, -0.7));
	mPoints.push_back(ofVec2f(-0.79, -0.94));
	mPoints.push_back(ofVec2f(0.98, -0.35));
	mPoints.push_back(ofVec2f(0.97, 0.98));
	updateBezierPoints();

}

Renderer::~Renderer()
{
	
}

void Renderer::draw(float elapsedTime, float dt)
{
	mx = ofGetMouseX() / (float)ofGetWidth();
	my = ofGetMouseY() / (float)ofGetHeight();
	
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
	glBindTexture(GL_TEXTURE_2D, mParticleTex.getTextureData().textureID);
	
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	int N = 60000;
	map<int,int> pCount;
	map<int,int> qCount;
	for (int i=0; i<N; ++i)
	{
		int p = i%NUM_INSTRUMENTS;
		int q = (i/NUM_INSTRUMENTS)%NUM_INSTRUMENTS; // dest
		if (p<=q)
			continue;
		ofSeedRandom(i*12421232);
		float period = ofRandom(50.)+10;
		float phase = ofRandom(1.)*period;
		float t = fmod((elapsedTime+phase)/period, 1.f);
		t *= min(1.f, t+0.2f);
		ofVec2f const& orig = mState.instruments.at(p).pos;
		ofVec2f const& dest = mState.instruments.at(q).pos;
		float amount = mState.instruments.at(p).connections.at(q);
//		printf("p %d q %d amount %f\n", p, q, amount);
//		if (amount<0.5)
//			continue;
		ofVec2f point;
		if (hermite)
		{
			point = interpHermite(p, q, t);
		}
		else
		{
			point = interp(p, q, t);
		}
		ofSeedRandom((NUM_INSTRUMENTS*p + q)*123232);
		float noise = ofNoise(point.x, point.y, elapsedTime*(ofRandom(0.1,0.7))*.4);
		float noise2 = ofNoise(point.x, point.y, elapsedTime*(ofRandom(0.1, 0.7)))*(sin(elapsedTime)+0.5f);
		float noise3 = ofNoise(point.x, point.y, elapsedTime*0.007);
		noise *= noise*noise;
		ofSeedRandom(i*124212);
		float brightness = sq(ofRandom(1)*0.63) * (0.3+0.7*amount);
		float scaleFactor = 1.+2*cos(ofRandom(-0.5, 0.5)) * 0.7;
		float size = 0.012f *scaleFactor * amount;
		point += ofVec2f(cos(TWO_PI*noise+noise3), sin(TWO_PI*noise+noise3)) * (0.015+(noise2*0.02-0.05) + 0.03*noise3);
		float redness = 0.f;
		if (ofRandom(1)>0.94)
			redness = ofRandom(0.3, 0.7);
		glColor4f(0.94,1-redness*redness,1-redness, brightness);
		mParticleTex.draw(point, size, size);
	}
	glBindTexture(GL_TEXTURE_2D, NULL);
	
	if (mState.debugMode)
		drawDebugOverlay();
}

void Renderer::addPoint(ofVec2f const& point)
{
	mPoints.push_back(point);
	updateBezierPoints();
}

void Renderer::removePoint()
{
	if (!mPoints.empty())
	{
		mPoints.pop_back();
		updateBezierPoints();
	}
}

std::vector<ofVec2f> Renderer::points() const
{
	return mPoints;
}


struct VecComparer
{
	bool operator()(ofVec2f const* a, ofVec2f const* b)
	{
		return mDistSq[a] > mDistSq[b];
	}
	
	static map<ofVec2f const*, float> mDistSq;
};

map<ofVec2f const*, float> VecComparer::mDistSq;

void Renderer::updateBezierPoints()
{	
	// alternative method
	mBeziersPerInstrumentPair.clear();
	const int n = mState.instruments.size();
	auto const& instruments = mState.instruments;
	for (int i=0; i<n; ++i)
	{
		ofVec2f orig = instruments.at(i).pos;
		mBeziersPerInstrumentPair[i] = map<int, vector<ofVec2f> >();
		ofVec2f last = orig;
		for (int j=0; j<n; ++j)
		{
			ofVec2f dest = instruments.at(j).pos;
			const int NUM_CONTROL_POINTS = min<int>(4, mPoints.size());
			vector<ofVec2f> controlPoints = mPoints;
			vector<ofVec2f> pointsFromOrigin;
			pointsFromOrigin.push_back(orig);
			vector<ofVec2f> pointsFromDest;
			while (pointsFromOrigin.size()+pointsFromDest.size() < NUM_CONTROL_POINTS && !controlPoints.empty())
			{
				// find nearest point to either last control point or destination
				vector<ofVec2f>::iterator nearestControlPointIt = controlPoints.end();
				float nearestControlPointDist = 99999999999999;
				bool nearestFromDest;
				for (auto it=controlPoints.begin(); it!=controlPoints.end(); ++it)
				{
					float distSqFromLast = last.distanceSquared(*it);
					float distSqFromDest = dest.distanceSquared(*it);
					float overallDistance = sqrt(distSqFromLast + distSqFromDest);
					if (overallDistance < nearestControlPointDist)
					{
						nearestControlPointIt = it;
						nearestControlPointDist = overallDistance;
					}
				}
				assert(nearestControlPointIt != controlPoints.end());
				pointsFromOrigin.push_back(*nearestControlPointIt);
				last = *nearestControlPointIt;
				controlPoints.erase(nearestControlPointIt);
			}
			pointsFromDest.push_back(dest);
			for (auto v: pointsFromDest)
				pointsFromOrigin.push_back(v);
			mBeziersPerInstrumentPair[i][j] = pointsFromOrigin;
		}
	}
	
	
	
	
	return;
	
	if (0)
	{
	mBeziersPerInstrumentPair.clear();
	const int n = mState.instruments.size();
	for (int i=0; i<n; ++i)
	{
		ofVec2f orig = mState.instruments.at(i).pos;
		mBeziersPerInstrumentPair[i] = map<int, vector<ofVec2f> >();
		for (int j=0; j<n; ++j)
		{
			ofVec2f dest = mState.instruments.at(j).pos;
			priority_queue<ofVec2f*, vector<ofVec2f*>, VecComparer> queue;
			for (ofVec2f& v: mPoints)
			{
				float d = orig.distanceSquared(v);
				VecComparer::mDistSq[&v] = d;
				queue.push(&v);
			}
			
			const int NUM_CONTROL_POINTS = min<int>(4, mPoints.size());
			// plus start and end
			std::vector<ofVec2f> bezierPoints;
			bezierPoints.reserve(NUM_CONTROL_POINTS+2);
			bezierPoints.push_back(orig);
			for (int i=0; i<NUM_CONTROL_POINTS; ++i)
			{
				bezierPoints.push_back(*queue.top());
				queue.pop();
			}
			if (bezierPoints.size() < 2) // in case we have no control points
			{
				bezierPoints.push_back(orig);
				bezierPoints.push_back(dest);
			}
			bezierPoints.push_back(dest);
			if (bezierPoints.size() < 4)
			{
				bezierPoints.push_back(dest);
			}
			mBeziersPerInstrumentPair[i][j] = bezierPoints;
		}
	}
	}
}

ofVec2f Renderer::interp(int inst0, int inst1, float t)
{
	std::vector<ofVec2f> const& bezierPoints = mBeziersPerInstrumentPair[inst0][inst1];
	// we need 4 points for a bezier segment
	const int numSegments = bezierPoints.size() - 3;
	int segment = min(numSegments-1, int(t*numSegments));
	float p = t*numSegments - segment;
	assert(0<=p && p<=1);
	assert(segment < bezierPoints.size()-3);
	return bezierInterp(bezierPoints[segment], bezierPoints[segment+1], bezierPoints[segment+2], bezierPoints[segment+3], p);

}

ofVec2f Renderer::interp(ofVec2f const& orig, ofVec2f const& dest, float t)
{
	priority_queue<ofVec2f const*, vector<ofVec2f const*>, VecComparer> queue;
	for (ofVec2f const& v: mPoints)
	{
		float d = orig.distanceSquared(v);
		VecComparer::mDistSq[&v] = d;
		queue.push(&v);
	}
	
	const int NUM_CONTROL_POINTS = min<int>(4, mPoints.size());
	// plus start and end
	std::vector<ofVec2f const*> bezierPoints;
	bezierPoints.reserve(NUM_CONTROL_POINTS+2);
	bezierPoints.push_back(&orig);
	for (int i=0; i<NUM_CONTROL_POINTS; ++i)
	{
		bezierPoints.push_back(queue.top());
		queue.pop();
	}
	bezierPoints.push_back(&dest);
	if (bezierPoints.size() < 4) // in case we have no control points
	{
		bezierPoints.push_back(&orig);
		bezierPoints.push_back(&dest);
	}
	// we need 4 points for a bezier segment
	const int numSegments = bezierPoints.size() - 3;
	int segment = min(numSegments-1, int(t*numSegments));
	float p = t*numSegments - segment;
	assert(0<=p && p<=1);
	assert(segment < bezierPoints.size()-3);
	return bezierInterp(*bezierPoints[segment], *bezierPoints[segment+1], *bezierPoints[segment+2], *bezierPoints[segment+3], p);
	

}

void Renderer::drawQuad(ofVec2f const& pos, ofVec2f const& size)
{
	glPushMatrix();
	ofTranslate(pos);
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

void Renderer::drawDebugOverlay()
{
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for (Instrument inst: mState.instruments)
	{
		for (int i=0; i<mState.instruments.size(); ++i)
		{
			float f = 15*inst.connections.at(i);
			glColor4f(1,1,1,min(1.f,f)*0.8);
			ofSetLineWidth(f);
			ofLine(inst.pos, mState.instruments.at(i).pos);
		}
	}
	glColor4f(1,1,1,0.8);
	for (Instrument inst: mState.instruments)
	{
		ofEllipse(inst.pos, 0.2, 0.2);
	}
	ofSetLineWidth(1.f);
	glColor4f(0,0,0,1);
	for (Instrument inst: mState.instruments)
	{
		ofDrawBitmapString(inst.name, inst.pos - ofVec2f(0.04, 0));
	}
	glColor4f(1,0,0,0.5);
	for (ofVec2f point: mPoints)
	{
		ofEllipse(point, 0.1, 0.1);
	}
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
	ofVec2f tangent0 = points[segment+1]-points[segment];
	ofVec2f tangent1 =
		segment+2>=points.size()? ofVec2f()
								: points[segment+2]-points[segment];
	return hermiteSpline(points[segment], tangent0, points[segment+1], tangent1, p);
	
}

ofVec2f Renderer::interpHermite(int inst0, int inst1, float t) const
{
	vector<ofVec2f> const& points =mBeziersPerInstrumentPair.at(inst0).at(inst1);
	return hermiteSpline(points, t);
}



