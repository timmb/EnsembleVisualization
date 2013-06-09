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
: debugDraw(false)
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
//
//	
//	
////	ofEnableNormalizedTexCoords();
////	ofImage particleImage;
//	const int dim = 64;
////	particleImage.allocate(dim, dim, OF_IMAGE_GRAYSCALE);
//	GLfloat particleImage[dim*dim*4];
//	for (int i=0; i<dim; i++)
//	{
//		float y = float(2*i)/dim - 1.f;
//		for (int j=0; j<dim; j++)
//		{
//			float x = float(2*j)/dim - 1.f;
//			float invRadius = max(0.f,1.f-ofVec2f(x, y).length());
////			particleImage.setColor(j, i, ofColor(ofColor::limit()*(1.f-radius)));
//			for (int k=0; k<4; ++k) {
//				int index = 4*(i*dim+j)+k;
//				printf("index %d\n", index);
//				if (index==10860)
//					int x=0;
//				printf("radius %f\n", invRadius);
//				particleImage[4*(i*dim+j)+k] = invRadius;// (GLubyte) int(255*invRadius);
////				printf("brightness %f\n", (unsigned char) (255*invRadius));
//				printf("particleimage %f\n", particleImage[4*(i*dim+j)+k]);
////				particleImage[4*(i*dim+j)+k] = 255- particleImage[4*(i*dim+j)+k];
//			}
//		}
//	}
////	particleImage.loadImage("/Users/tim/Pictures/hiroshi-sugimoto-e69d89e69cace58d9ae58fb8-25.jpg");
////	mParticleTex.allocate(particleImage);
////	mParticleTex.loadData(particleImage);
////	tmp.loadImage("/Users/tim/Pictures/hiroshi-sugimoto-e69d89e69cace58d9ae58fb8-25.jpg");
//	glActiveTexture(GL_TEXTURE0);
//	glGenTextures(1, &mTex);
//	glBindTexture(GL_TEXTURE_2D, mTex);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_FLOAT, particleImage);
////	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, dim, dim, GL_RGBA, GL_FLOAT, particleImage );
//	glBindTexture(GL_TEXTURE_2D, NULL);
//	printf("mtex %d\n", mTex);
}

Renderer::~Renderer()
{
	
}

void Renderer::draw(float elapsedTime, float dt)
{
	mx = ofGetMouseX() / (float)ofGetWidth();
	my = ofGetMouseY() / (float)ofGetHeight();
//	printf("mx %f my %f\n", mx, my);
	
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// normalized coordinates: 2x2 square centred at origin
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
//	mParticleTex.loadData(tmp);
//	mParticleTex.draw(ofRectangle(-1,-1,2,2));
//	glColor4f(1,1,1,1);
//	glEnable(GL_TEXTURE_2D);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, mTex);
//	drawQuad(ofVec2f(), ofVec2f(2,2));
//	glBindTexture(GL_TEXTURE_2D, 0);
//	return;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
//	mParticleTex.bind();
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, mTex);
	glBindTexture(GL_TEXTURE_2D, mParticleTex.getTextureData().textureID);
	
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	int N = 30000;
	map<int,int> pCount;
	map<int,int> qCount;
//	GLfloat particles[2*N];
	if (ofGetFrameNum()%10==0)
		cout << mx << "  " << my << endl;
	for (int i=0; i<N; ++i)
	{
		ofSeedRandom(i*12421232);
		float period = ofRandom(50.)+10;
		float phase = ofRandom(1.)*period;
		float t = fmod((elapsedTime+phase)/period, 1.f);
		t *= min(1.f, t+0.2f);
		int p = i%NUM_INSTRUMENTS;
		int q = (i/NUM_INSTRUMENTS)%NUM_INSTRUMENTS; // dest
		ofVec2f const& orig = mState.instruments.at(p).pos;
		ofVec2f const& dest = mState.instruments.at(q).pos;
//		float deviation = ofRandom(mx-0.3, mx);
//		deviation = pow(deviation, my*5);
//		ofVec2f mid = dest.middled(orig) + (dest-orig).getRotatedRad(HALF_PI).normalized()*deviation;
//		interp.push_back(mid);
//		interp.push_back(dest);
//		ofVec2f point = interp.sampleAt(t);
		ofVec2f point = interp(p, q, t);
		ofSeedRandom((NUM_INSTRUMENTS*p + q)*123232);
		float noise = ofNoise(point.x, point.y, elapsedTime*(ofRandom(0.1,0.7))*.4);
		float noise2 = ofNoise(point.x, point.y, elapsedTime*(ofRandom(0.1, 0.7)))*(sin(elapsedTime)+0.5f);
		float noise3 = ofNoise(point.x, point.y, elapsedTime*0.007);
		noise *= noise*noise;
		ofSeedRandom(i*124212);
		float brightness = sq(ofRandom(1)*0.5);
		float scaleFactor = 1.+2*cos(ofRandom(-0.5, 0.5)) * 0.5;
		float size = 0.012f *scaleFactor;
		point += ofVec2f(cos(TWO_PI*noise+noise3), sin(TWO_PI*noise+noise3)) * (0.015+(noise2*0.02-0.05) + 0.03*noise3);
//		point += my*ofVec2f(cos(p*q+ofRandom(TWO_PI)), sin(p*q+ofRandom(TWO_PI)));
//		point += 1.f/exp(my*p*q+ofRandom(TWO_PI))*(ofVec2f() - point);
//		particles[2*i] = point.x;
//		particles[2*i+1] = point.y;
//		ofRect(point, 0.02, 0.02);
//		drawQuad(point, ofVec2f(0.02, 0.02));
		float redness = 0.f;
		if (ofRandom(1)>0.94)
			redness = ofRandom(0.3, 0.7);
		glColor4f(0.94,1-redness*redness,1-redness, brightness);
		mParticleTex.draw(point, size, size);
	}
//	glPointSize(3);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_FLOAT, 0, particles);
//	glDrawArrays(GL_POINTS, 0, N);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	mParticleTex.unbind();
	glBindTexture(GL_TEXTURE_2D, NULL);
	
	if (debugDraw)
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
		return mDistSq[a] < mDistSq[b];
	}
	
	static map<ofVec2f const*, float> mDistSq;
};

map<ofVec2f const*, float> VecComparer::mDistSq;

void Renderer::updateBezierPoints()
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
			bezierPoints.push_back(dest);
			if (bezierPoints.size() < 4) // in case we have no control points
			{
				bezierPoints.push_back(orig);
				bezierPoints.push_back(dest);
			}
			mBeziersPerInstrumentPair[i][j] = bezierPoints;
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
//	glBegin(GL_TRIANGLE_FAN);
//	for (int i=0; i<4; ++i)
//	{
//		glTexCoord2f(uvs[2*i], uvs[2*i+1]);
//		glVertex2f(vertices[2*i], vertices[2*i+1]);
//	}
//	glEnd();
//	glPopMatrix();
//	return;
	
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
	glColor4f(1,1,1,0.8);
	for (Instrument inst: mState.instruments)
	{
		ofEllipse(inst.pos, 0.2, 0.2);
	}
	glColor4f(1,0,0,0.5);
	for (ofVec2f point: mPoints)
	{
		ofEllipse(point, 0.1, 0.1);
	}
}