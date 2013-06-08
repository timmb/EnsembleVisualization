//
//  Renderer.h
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once

#include "State.h"

class Renderer
{
public:
	
	void setState(State const& newState);

	void draw(float elapsedTime, float dt);
	
private:
	void drawDebugOverlay();
	State mState;
	
};