//
//  Renderer.cpp
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "Renderer.h"
#include "ofMain.h"

void Renderer::setState(State const& newState)
{
	mState = newState;
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
	
	drawDebugOverlay();
}

void Renderer::drawDebugOverlay()
{
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	glColor4f(1,1,1,0.8);
	for (Instrument inst: mState.instruments)
	{
		ofEllipse(inst.pos, 0.2, 0.2);
	}
}