#pragma once

#include "ofMain.h"
#include "Renderer.h"
#include "State.h"
#include "OscReceiver.h"

class EnsembleVisualization : public ofBaseApp
{
public:
	EnsembleVisualization();
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
private:
	float mElapsedTime;
	float mDt;
	Renderer* mRenderer;
//	OscReceiver mOscReceiver;
};
