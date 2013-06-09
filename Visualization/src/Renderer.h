//
//  Renderer.h
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once

#include "State.h"

struct Particle
{
	
	
};


class Renderer
{
public:
	Renderer();
	virtual ~Renderer();
	
	void setState(State const& newState);

	void draw(float elapsedTime, float dt);
	
	std::vector<ofVec2f> points;
	bool debugDraw;
	
private:
	void drawQuad(ofVec2f const& pos, ofVec2f const& size);
	void drawDebugOverlay();
	ofVec2f interp(ofVec2f const& orig, ofVec2f const& dest, float t);
	State mState;
	ofTexture mParticleTex;
//	ofImage tmp;
	GLuint mTex;
	
	float mx, my;
};