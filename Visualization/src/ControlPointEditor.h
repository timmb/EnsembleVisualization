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
	/// Will only draw when setupmode is enabled
	void draw(float elapsedTime, float dt);
	
	void save();
	void load();
	
	void setEnableSetupMode(bool enabled);
	
	void keyPressed(int key, bool ctrlPressed, bool altPressed);
	void mousePressed(ofVec2f const& pos, int button);
	
private:
	/// Call to update stuff when something changes
	void notify();
	void clearPoints();
	
	bool mIsInSetupMode;
	Renderer* mRenderer;
	string mJsonFilename;
	std::map<int, std::map<int, std::vector<ofVec2f> > > mControlPoints;

	string mStatus;
	string getName(int instrumentNumber) const;
	
	bool mWasDebugEnabledLastFrame;
	
	// editing mode:
	static const int NONE = -1;
	int mOriginInstrument;
	int mDestInstrument;
	vector<bool> mInstrumentVisibility;

};

