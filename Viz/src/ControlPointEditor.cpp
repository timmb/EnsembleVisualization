//
//  ControlPointEditor.cpp
//  Visualization
//
//  Created by Tim Murray-Browne on 21/06/2013.
//
//

#include "ControlPointEditor.h"
#include "json.h"
#include "cinder/Utilities.h"
#include <ostream>
#include <fstream>

using namespace ci;
using namespace std;

ControlPointEditor::ControlPointEditor()
: mRenderer(NULL)
, mIsInSetupMode(false)
, mInstrumentVisibility(NUM_INSTRUMENTS, true)
{
	for (int i=0; i<NUM_INSTRUMENTS; i++)
	{
		mControlPoints[i] = map<int, std::vector<ci::Vec2f> >();
		for (int j=0; j<NUM_INSTRUMENTS; j++)
		{
			mControlPoints[i][j] = std::vector<ci::Vec2f>();
		}
	}
	mEditingInstruments[0] = NONE;
	mEditingInstruments[1] = NONE;
}

ControlPointEditor::~ControlPointEditor()
{
	save();
}

void ControlPointEditor::setEnableSetupMode(bool enabled)
{
	mIsInSetupMode = enabled;
	notify();
}

void ControlPointEditor::setup(Renderer* renderer)
{
	mJsonFilename = ci::app::getAssetPath("control_points.json").string();
	mRenderer = renderer;
	load();
	notify();
}

void ControlPointEditor::save()
{
	using namespace Json;
	Value jRoot;
	Value& jPoints = jRoot["control points"];
	for (int i=0; i<NUM_INSTRUMENTS; i++)
		for (int j=0; j<NUM_INSTRUMENTS; j++)
		{
			jPoints[i][j] = Value(arrayValue);
			for (int k=0; k<mControlPoints[i][j].size(); k++)
				for (int l=0; l<mControlPoints[i][j][k].DIM; l++)
					jPoints[i][j][k][l] = mControlPoints[i][j][k][l];
		}
	ofstream out;
	out.open(mJsonFilename.c_str());
	if (out.good())
	{
		out << jRoot;
		out.close();
	}
	if (out.good())
	{
		cout << "Successfully written to file "<<mJsonFilename<<endl;
	}
	else
	{
		cout << "ERROR when writing to file "<<mJsonFilename<<endl;
	}
}

void ControlPointEditor::load()
{
	cout << "Loading "<<mJsonFilename<<"..."<<endl;
	using namespace Json;
	Value jRoot;
	Reader reader;
	ifstream in;
	in.open(mJsonFilename.c_str());
	bool success = reader.parse(in, jRoot);
	if (!success)
	{
		cout << "WARNING: Could not open "<<mJsonFilename<<":\n"
		<< reader.getFormatedErrorMessages() << endl;
		return;
	}
	Value& jControlPoints = jRoot["control points"];
	if (jControlPoints.isNull())
	{
		cout << "WARNING: Could not read "<<mJsonFilename<<" as could not find \"control points\" element"<<endl;
		success = false;
		return;
	}
	for (int i=0; i<NUM_INSTRUMENTS; i++)
	{
		Value& jPointsOrig = jControlPoints[i];
		if (jPointsOrig.isNull())
		{
			cout << "WARNING: Could not read control points from origin insturment "<<i<<endl;
			continue;
		}
		for (int j=0; j<NUM_INSTRUMENTS; j++)
		{
			if (i==j)
				continue;
			Value& jPoints =jPointsOrig[j];
//			int n = jPointsOrig.size();
			if (jPoints.isNull())
			{
				cout << "WARNING: Could not read control points from instrument "<<i<<" with destination "<<j<<" from "<<mJsonFilename<<endl;
				success = false;
				continue;
			}
			mControlPoints[i][j].clear();
			for (int k=0; k<jPoints.size(); k++)
			{
				Value jVec = jPoints[k];
				if (jVec.isNull() || jVec.size()!= ci::Vec2f::DIM)
				{
					cout << "WARNING: Could not read control point "<<k<<" from instrument "<<i<<" to "<<j<<" from "<<mJsonFilename<<endl;
					success = false;
					continue;
				}
				ci::Vec2f v;
				assert(v.DIM == jVec.size());
				bool skipV = false;
				for (int l=0; l<jVec.size(); l++)
				{
					if (!jVec[l].isConvertibleTo(realValue))
					{
						cout << "WARNING: Could not read component "<<l<<" from control point "<<k<<" from instrument "<<i<<" to "<<j<<" from "<<mJsonFilename<<endl;
						skipV = true;
						success = false;
						break;
					}
					v[l] = (float) jVec[l].asDouble();
				}
				if (!skipV)
				{
					mControlPoints[i][j].push_back(v);
				}
			}
		}
	}

	if (success)
	{
		cout << mJsonFilename << " loaded successfully."<<endl;
	}
	notify();
}

void ControlPointEditor::draw(float elapsedTime, float dt)
{
	
	State state = mRenderer->state();
	if (mWasDebugEnabledLastFrame != state.debugMode)
	{
		notify();
		mWasDebugEnabledLastFrame = state.debugMode;
	}
	if (!state.debugMode)
		return;
	auto instruments = state.instruments;
	
	ci::ColorAf editCol(0,1,0,0.8);
	ci::ColorAf visibleCol(1,1,1,0.4);
//	float p = sin(elapsedTime)*.5 + .5;
	ci::ColorAf editingControlPoint(.4,1.,.4,0.8);
	ci::ColorAf notEditingControlPoint(.5,.5,1,0.2);
	
	// normalized coordinates: 2x2 square centred at origin
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);

	ci::gl::enableAdditiveBlending();
	//	glColor4f(1,1,1,0.8);
	assert(instruments.size() == mInstrumentVisibility.size());
	for (int i=0; i<instruments.size(); ++i)
	{
		if (!mInstrumentVisibility.at(i))
			continue;
		Instrument inst = instruments.at(i);
		if (i==mEditingInstruments[0] || i==mEditingInstruments[1])
			gl::color(editCol);
		else
			gl::color(visibleCol);
		gl::drawSolidEllipse(inst.pos, 0.2, 0.2, 30);
	}
	
	gl::lineWidth(1.f);
	glColor4f(0,0,0,1);
	for (int i=0; i<instruments.size(); ++i)
	{
		Instrument const& inst = instruments.at(i);
		gl::pushModelView();
		gl::translate(inst.pos);
		gl::scale(0.004, -0.004);
		tmb::drawString(ci::toString(i)+" "+inst.name, Vec2f(), true);
		gl::popModelView();
//		ofDrawBitmapString(ofToString(i)+" "+inst.name, inst.pos - ci::Vec2f(0.04, 0));
	}
	//	glColor4f(1,0,0,0.5);
	for (int i=0; i<instruments.size(); ++i)
	{
		for (int j=0; j<i; ++j)
		{
			if (!mInstrumentVisibility.at(i) || !mInstrumentVisibility.at(j))
				continue;
			auto const& points = mControlPoints[i][j];
			if (isEditing(i) && isEditing(j))
				gl::color(editingControlPoint);
			else if (mInstrumentVisibility.at(i) && mInstrumentVisibility.at(j))
				gl::color(notEditingControlPoint);
			else
				continue;
			for (ci::Vec2f const& point: points)
				gl::drawSolidEllipse(point, 0.1, 0.1, 25);
		}
	}
	
	// status text
	gl::setMatricesWindow(app::getWindowWidth(), app::getWindowHeight());
//	ofSetupScreen();
	gl::color(255, 163, 183);
	tmb::drawString(mStatus, ci::Vec2f(10, 10), false);
	gl::enableAlphaBlending();

}

void ControlPointEditor::keyPressed(ci::app::KeyEvent event)
{
	char key = event.getChar();
	bool ctrlPressed = event.isControlDown();
	bool altPressed = event.isAltDown();
//	bool shiftPressed = event.isShiftDown();
	
	if (mRenderer->state().debugMode)
	{
		if (key=='d')
		{
			editInstrument(NONE);
			editInstrument(NONE);
		}
		if (key=='-' || ('0' <= key && key < '8'))
		{
			int inst = key=='-'? NONE : key - '0';
			if (!ctrlPressed && !altPressed && mIsInSetupMode)
			{
				editInstrument(inst);
			}
			else if (ctrlPressed && !altPressed)
			{
				// change all when - is pressed
				if (inst == -1)
					mInstrumentVisibility.assign(mInstrumentVisibility.size(), !mInstrumentVisibility.at(0));
				else
					mInstrumentVisibility.at(inst) = !mInstrumentVisibility.at(inst);
			}
		}
		else if (mIsInSetupMode && event.getCode()==cinder::app::KeyEvent::KEY_BACKSPACE && ctrlPressed)
		{
			clearPoints();
		}
		else if (key=='e')
		{
			mIsInSetupMode = !mIsInSetupMode;
			if (!mIsInSetupMode)
			{
				editInstrument(NONE);
				editInstrument(NONE);
			}
		}
		else if (key=='c')
		{
			mRenderer->setEnableDrawConnectionsDebug(!mRenderer->isDrawConnectionsDebugEnabled());
		}
	}
	notify();
}

void ControlPointEditor::clearPoints()
{
	for (auto& orig: mControlPoints)
		for (auto& dest: orig.second)
			dest.second.clear();
	cout << "All control points cleared."<<endl;
}

void ControlPointEditor::mousePressed(ci::Vec2f const& pos, int button)
{
	static const int LEFT = 0;
	static const int RIGHT = 2;
	if (!isEditing(NONE))
	{
		// always consider points from lowest value instrument to
		// highest value instrument
		int inst0 = min(mEditingInstruments[0], mEditingInstruments[1]);
		int inst1 = max(mEditingInstruments[0], mEditingInstruments[1]);
		
		auto& points0 = mControlPoints.at(inst0).at(inst1);
		auto& points1 = mControlPoints.at(inst1).at(inst0);
		if (button==LEFT)
		{
			points0.push_back(pos);
			// higher valued instrument we insert the control points
			// in reverse order
			points1.insert(points1.begin(), pos);
		}
		else if (button==RIGHT)
		{
			if (!points0.empty())
				points0.pop_back();
			if (!points1.empty())
				points1.erase(points1.begin());
		}
	}
	notify();
}

void ControlPointEditor::notify()
{
	mRenderer->setControlPoints(mControlPoints);
	mStatus = "S to save, L to load, C to draw connections, P to print state, R to randomize state,\nM for maximal state, space to toggle debug interface, E to toggle control point editor";
	if (mIsInSetupMode)
		mStatus += "\nEditing: "+toString(mEditingInstruments[0])+" "
		+ getName(mEditingInstruments[0])+" and "+toString(mEditingInstruments[1])+" "
		+ getName(mEditingInstruments[1])+"\n<num> to select instruments, control <num> to change visibility"
		+ "\n(use '-' for no/all instrument(s)), d to deselect, Ctrl Backspace to clear all control points\nStart points at low-numbered instrument";
}

string ControlPointEditor::getName(int instrumentNumber) const
{
	if (instrumentNumber<0)
		return "(none)";
	else if (instrumentNumber<NUM_INSTRUMENTS)
		return mRenderer->state().instruments.at(instrumentNumber).name;
	else
		return "error";
}

void ControlPointEditor::editInstrument(int inst)
{
	if (inst!=NONE && isEditing(inst))
		return;
	if (mEditingInstruments[1] != NONE || inst==NONE)
		mEditingInstruments[0] = mEditingInstruments[1];
	mEditingInstruments[1] = inst;
}
