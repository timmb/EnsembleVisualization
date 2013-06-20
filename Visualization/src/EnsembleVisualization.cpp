#include "EnsembleVisualization.h"

EnsembleVisualization::EnsembleVisualization()
: mElapsedTime(-1.)
, mDt(1.)
, mStabilizerHost("127.0.0.1")
, mStabilizerPort(1123)
, mListenPort(12378)
, mRenderer(NULL)
{
}

//-------------------------------------------------------------
void EnsembleVisualization::setup(){
	mRenderer = new Renderer;
	mRenderer->setState(State::randomState(0));
	mOscReceiver.setup(mListenPort, mStabilizerHost, mStabilizerPort);
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
	if (ofGetFrameNum()%30==0)
	{
		cout << mOscReceiver.status() << endl;
		cout << "State at time "<<mElapsedTime<<"\n"
		<< mOscReceiver.state() << endl;
	}
}

//--------------------------------------------------------------
void EnsembleVisualization::draw(){
	mRenderer->draw(mElapsedTime, mDt);
}

//--------------------------------------------------------------
void EnsembleVisualization::keyPressed(int key){
	if (key=='s')
		mRenderer->setState(State::randomState(ofGetElapsedTimef()));
	if (key==' ')
		mRenderer->debugDraw = !mRenderer->debugDraw;
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