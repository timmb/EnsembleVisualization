#include "EnsembleVisualization.h"
#include <GLUT/GLUT.h>

EnsembleVisualization::EnsembleVisualization()
: mElapsedTime(-1.)
, mDt(1.)
, mStabilizerHost("127.0.0.1")
, mStabilizerPort(1123)
, mListenPort(12378)
, mRenderer(NULL)
, mInstrumentVisibility(NUM_INSTRUMENTS, true)
, mCurrentOrig(-1)
, mCurrentDest(-1)
{
}

//-------------------------------------------------------------
void EnsembleVisualization::setup(){
	mRenderer = new Renderer;
	mRenderer->setState(State::randomState(0));
	mOscReceiver.setup(mListenPort, mStabilizerHost, mStabilizerPort);
	mEditor.setup(mRenderer);
}

//--------------------------------------------------------------
void EnsembleVisualization::update(){
	float t = ofGetElapsedTimef();
	float dt = t - mElapsedTime;
	mElapsedTime = t;
	mOscReceiver.update(mElapsedTime, dt);
	if (mOscReceiver.hasNewState())
	{
		mRenderer->setState(mOscReceiver.state());
	}
	// debugging
	if (0)
	{
		if (ofGetFrameNum()%30==0)
		{
			cout << mOscReceiver.status() << endl;
			cout << "State at time "<<mElapsedTime<<"\n"
			<< mOscReceiver.state() << endl;
		}
		cout << 1.f/dt << endl;
	}
}

//--------------------------------------------------------------
void EnsembleVisualization::draw(){
	mRenderer->draw(mElapsedTime, mDt);
	mEditor.draw(mElapsedTime, mDt);
}

//--------------------------------------------------------------
void EnsembleVisualization::keyPressed(int key){
	bool ctrl = glutGetModifiers() & GLUT_ACTIVE_CTRL;
	bool alt = glutGetModifiers() & GLUT_ACTIVE_ALT;
	
	if (key=='r')
	{
		bool d = mOscReceiver.state().debugMode;
		State state = State::randomState(ofGetElapsedTimef());
		state.debugMode = d;
		mOscReceiver.setState(state);
	}
	else if (key=='s')
		mEditor.save();
	else if (key=='l')
		mEditor.load();
	if (key==' ')
		mOscReceiver.toggleDebugMode();
	if (key=='p')
		cout << "Renderer state:\n"<<mRenderer->state()<<endl;
	mEditor.keyPressed(key, ctrl, alt);
}



//--------------------------------------------------------------
void EnsembleVisualization::keyReleased(int key){

}

//--------------------------------------------------------------
void EnsembleVisualization::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void EnsembleVisualization::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void EnsembleVisualization::mousePressed(int x, int y, int button){
	ofVec2f pos = ofVec2f(float(x)/ofGetWidth() * 2 - 1, float(ofGetHeight() - y)/ofGetHeight() * 2 - 1);
	mEditor.mousePressed(pos, button);
}

std::string EnsembleVisualization::getName(int instrumentNumber)
{
	if (instrumentNumber==-1)
		return "(none)";
	else if (instrumentNumber<NUM_INSTRUMENTS)
		return mOscReceiver.state().instruments.at(instrumentNumber).name;
	else return "error";
}


//--------------------------------------------------------------
void EnsembleVisualization::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void EnsembleVisualization::windowResized(int w, int h){

}

//--------------------------------------------------------------
void EnsembleVisualization::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void EnsembleVisualization::dragEvent(ofDragInfo dragInfo){ 

}