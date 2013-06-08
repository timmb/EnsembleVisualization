//
//  OscReceiver.h
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once
#include <string>
#include "State.h"
#include "ofxOsc.h"

class OscReceiver
{
public:
	OscReceiver();
	void setup(int port);
	void update(float elapsedTime, float dt);
	
	bool hasNewState() const;
//	State state() const;


	/// For debugging
	std::string status() const;
	
private:
	ofxOscReceiver mOsc;
//	State mState;
	
};