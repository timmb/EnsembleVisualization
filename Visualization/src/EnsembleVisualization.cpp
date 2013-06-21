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
	updateStatus();
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
	mRenderer->setEditingMode(mInstrumentVisibility, mCurrentOrig, mCurrentDest);
}

//--------------------------------------------------------------
void EnsembleVisualization::draw(){
	mRenderer->draw(mElapsedTime, mDt);
	ofSetupScreen();
	if (mOscReceiver.state().debugMode)
	{
		ofSetColor(255, 163, 183);
		ofDrawBitmapString(mStatus, ofVec2f(10, 10));
	}
}

//--------------------------------------------------------------
void EnsembleVisualization::keyPressed(int key){
	if (key=='r')
		mOscReceiver.setState(State::randomState(ofGetElapsedTimef()));
	else if (key=='s')
		mEditor.save();
	else if (key=='l')
		mEditor.load();
	if (key==' ')
		mOscReceiver.toggleDebugMode();
	if (key=='-' || ('0'<=key && key < '8'))
	{
		int inst;
		if (key=='-')
			inst = -1;
		else
			inst = key - '0';
		int mods = glutGetModifiers();
		cout << mods << endl;
		// set visibility
		if (mods == 0)
		{
			if (key!=-1)
				mInstrumentVisibility.at(inst) = !mInstrumentVisibility.at(inst);
			else
			{
				bool visible = !mInstrumentVisibility.at(0);
				for (int i=0; i<mInstrumentVisibility.size(); ++i)
					mInstrumentVisibility[i] = visible;
			}
		}
		if (mods & GLUT_ACTIVE_CTRL)
		{
			mCurrentOrig = inst;
		}
		if (mods & GLUT_ACTIVE_ALT)
		{
			mCurrentDest = inst;
		}
		updateStatus();
	}
	if (key=='p')
		cout << "Renderer state:\n"<<mRenderer->state()<<endl;
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
	if (button==0)
	{
	ofVec2f p = toNorm(x,y);
	mRenderer->addPoint(p);
	}
	else if (button==2)
	{
		mRenderer->removePoint();
	}
	std::cout << "Points" << mRenderer->points() << endl;
}

std::string EnsembleVisualization::getName(int instrumentNumber)
{
	if (instrumentNumber==-1)
		return "(none)";
	else if (instrumentNumber<NUM_INSTRUMENTS)
		return mOscReceiver.state().instruments.at(instrumentNumber).name;
	else return "error";
}

void EnsembleVisualization::updateStatus()
{
	mStatus = "Origin: "+ofToString(mCurrentOrig)+" "
	+ getName(mCurrentOrig)+", Dest "+ofToString(mCurrentDest)+" "
	+ getName(mCurrentDest)+"\n<num> to change visibility, control <num> to change origin, alt <num> to change dest"
	+ "\nuse '-' for no instrument";
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