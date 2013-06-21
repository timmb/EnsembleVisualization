//
//  ControlPointEditor.h
//  Visualization
//
//  Created by Tim Murray-Browne on 21/06/2013.
//
//

#pragma once

#include "Common.h"
#include "Renderer.h"


class ControlPointEditor
{
public:
	ControlPointEditor();
	~ControlPointEditor();
	
	void setup(Renderer* renderer);
	void update(float elapsedTime, float dt);
	
	void save();
	void load();
	
private:
	Renderer* mRenderer;
	string mJsonFilename;
	std::map<int, std::map<int, std::vector<ofVec2f> > > mControlPoints;
	
};