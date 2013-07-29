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
	
	void keyPressed(ci::app::KeyEvent event);
	/// Pos is in normalized coordinates
	/// button is 0 for LEFT, 2 for RIGHT
	void mousePressed(ci::Vec2f const& pos, int button);
	/// Pos is in normalized coordinates
	/// button is 0 for LEFT, 2 for RIGHT
	void mouseDragged(ci::Vec2f const& pos, int button);
	void mouseReleased(int button);
	
	tmb::Quad warpQuad() const { return mWarpQuad; }
	
private:
	/// Call to update stuff when something changes
	void notify();
	void clearPoints();
	
	bool mIsInSetupMode;
	bool mIsInWarpMode;
	Renderer* mRenderer;
	std::string mJsonFilename;
	std::map<int, std::map<int, std::vector<ci::Vec2f> > > mControlPoints;
	
	// warp editor
	/// the quad things get drawn to
	tmb::Quad mWarpQuad;
	/// the noramlized coordinate quad (-1 to +1)
	tmb::Quad mOriginalQuad;
	enum Corner_ { TL, TR, BR, BL };
	typedef int Corner;
	Corner mCurrentlyBeingDragged;
	ci::Vec2f mDragOffset;
	void updateWarpTransform();

	std::string mStatus;
	std::string getName(int instrumentNumber) const;
	
	bool mWasDebugEnabledLastFrame;
	
	// editing mode:
	static const int NONE = -1;
	int mEditingInstruments[2];
	void editInstrument(int inst);
	bool isEditing(int inst) { return mEditingInstruments[0]==inst || mEditingInstruments[1] == inst; }
	std::vector<bool> mInstrumentVisibility;

	template <typename T>
	void push_front(std::vector<T>& dest, T const& value);
};

