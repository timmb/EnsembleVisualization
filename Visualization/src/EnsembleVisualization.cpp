#include "EnsembleVisualization.h"

EnsembleVisualization::EnsembleVisualization()
: mElapsedTime(-1.)
, mDt(1.)
{}

//-------------------------------------------------------------
void EnsembleVisualization::setup(){
	mRenderer.setState(State::randomState(0));
}

//--------------------------------------------------------------
void EnsembleVisualization::update(){
	float t =ofGetElapsedTimef();
	float dt = t - mElapsedTime;
	mElapsedTime = t;

}

//--------------------------------------------------------------
void EnsembleVisualization::draw(){
	mRenderer.draw(mElapsedTime, mDt);
}

//--------------------------------------------------------------
void EnsembleVisualization::keyPressed(int key){
	if (key=='s')
		mRenderer.setState(State::randomState(ofGetElapsedTimef()));
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