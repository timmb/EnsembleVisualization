#pragma once

#include "ofMain.h"
#include "Renderer.h"
#include "State.h"
#include "OscReceiver.h"
#include "ControlPointEditor.h"

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
	int mListenPort;
	std::string mStabilizerHost;
	int mStabilizerPort;
	
	float mElapsedTime;
	float mDt;
	Renderer* mRenderer;
	OscReceiver mOscReceiver;
	ControlPointEditor mEditor;
	
	// for setting the control points. only shown on debug
	void updateStatus();
	std::string getName(int instrumentNumber);
	std::string mStatus;
	/// which instrument control points are visible
	vector<bool> mInstrumentVisibility;
	/// being currently edited - set to -1 for none
	int mCurrentOrig;
	int mCurrentDest;
};
