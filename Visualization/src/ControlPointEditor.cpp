//
//  ControlPointEditor.cpp
//  Visualization
//
//  Created by Tim Murray-Browne on 21/06/2013.
//
//

#include "ControlPointEditor.h"
#include "json.h"

ControlPointEditor::ControlPointEditor()
: mRenderer(NULL)
, mIsInSetupMode(false)
, mOriginInstrument(NONE)
, mDestInstrument(NONE)
, mInstrumentVisibility(NUM_INSTRUMENTS, true)
{
	for (int i=0; i<NUM_INSTRUMENTS; i++)
	{
		mControlPoints[i] = map<int, std::vector<ofVec2f> >();
		for (int j=0; j<NUM_INSTRUMENTS; j++)
		{
			mControlPoints[i][j] = std::vector<ofVec2f>();
		}
	}
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
	mJsonFilename = ofToDataPath("control_points.json");
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
			int n = jPointsOrig.size();
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
				if (jVec.isNull() || jVec.size()!= ofVec2f::DIM)
				{
					cout << "WARNING: Could not read control point "<<k<<" from instrument "<<i<<" to "<<j<<" from "<<mJsonFilename<<endl;
					success = false;
					continue;
				}
				ofVec2f v;
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
	
	ofFloatColor originCol(0,1,0,0.8);
	ofFloatColor destCol(1,0,0,0.8);
	ofFloatColor visibleCol(1,1,1,0.4);
	ofFloatColor editingControlPoint(.4,.4,1,0.8);
	ofFloatColor notEditingControlPoint(.5,.5,1,0.2);
	
	// normalized coordinates: 2x2 square centred at origin
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	//	glColor4f(1,1,1,0.8);
	assert(instruments.size() == mInstrumentVisibility.size());
	for (int i=0; i<instruments.size(); ++i)
	{
		if (!mInstrumentVisibility.at(i))
			continue;
		Instrument inst = instruments.at(i);
		if (i==mOriginInstrument)
			ofSetColor(originCol);
		else if (i==mDestInstrument)
			ofSetColor(destCol);
		else
			ofSetColor(visibleCol);
		ofEllipse(inst.pos, 0.2, 0.2);
	}
	ofSetLineWidth(1.f);
	glColor4f(0,0,0,1);
	for (int i=0; i<instruments.size(); ++i)
	{
		Instrument const& inst = instruments.at(i);
		ofDrawBitmapString(ofToString(i)+" "+inst.name, inst.pos - ofVec2f(0.04, 0));
	}
	//	glColor4f(1,0,0,0.5);
	for (int i=0; i<instruments.size(); ++i)
	{
		for (int j=0; j<instruments.size(); ++j)
		{
			if (!mInstrumentVisibility.at(i) || !mInstrumentVisibility.at(j))
				continue;
			auto const& points = mControlPoints[i][j];
			if (i==mOriginInstrument && j==mDestInstrument)
				ofSetColor(editingControlPoint);
			else if (mInstrumentVisibility.at(i) && mInstrumentVisibility.at(j))
				ofSetColor(notEditingControlPoint);
			else
				continue;
			for (ofVec2f const& point: points)
				ofEllipse(point, 0.1, 0.1);
		}
	}
	
	// status text
	ofSetupScreen();
	ofSetColor(255, 163, 183);
	ofDrawBitmapString(mStatus, ofVec2f(10, 10));

}

void ControlPointEditor::keyPressed(int key, bool ctrlPressed, bool altPressed)
{
	if (mRenderer->state().debugMode)
	{
		if ((key=='-' || ('0' <= key && key < '8')))
		{
			int inst = key=='-'? -1 : key - '0';
			if (ctrlPressed && mIsInSetupMode)
			{
				mOriginInstrument = inst;
				if (mDestInstrument == inst)
					mDestInstrument = NONE;
			}
			else if (altPressed && mIsInSetupMode)
			{
				mDestInstrument = inst;
				if (mOriginInstrument == inst)
					mOriginInstrument = NONE;
			}
			else if (!altPressed && !ctrlPressed)
			{
				// change all when - is pressed
				if (inst == -1)
					mInstrumentVisibility.assign(mInstrumentVisibility.size(), !mInstrumentVisibility.at(0));
				else
					mInstrumentVisibility.at(inst) = !mInstrumentVisibility.at(inst);
			}
		}
		else if (mIsInSetupMode && key==OF_KEY_BACKSPACE && ctrlPressed)
		{
			clearPoints();
		}
		else if (key=='e')
		{
			mIsInSetupMode = !mIsInSetupMode;
			if (!mIsInSetupMode)
			{
				mOriginInstrument = NONE;
				mDestInstrument = NONE;
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

void ControlPointEditor::mousePressed(ofVec2f const& pos, int button)
{
	static const int LEFT = 0;
	static const int RIGHT = 2;
	if (mOriginInstrument!=-1 && mDestInstrument!=-1 && mOriginInstrument!=mDestInstrument)
	{
		auto& points = mControlPoints.at(mOriginInstrument).at(mDestInstrument);
		if (button==LEFT)
			points.push_back(pos);
		else if (button==RIGHT && !points.empty())
			points.pop_back();
	}
	notify();
}

void ControlPointEditor::notify()
{
	mRenderer->setControlPoints(mControlPoints);
	mStatus = "S to save, L to load, C to draw connections, P to print state, R to randomize state,\nspace to toggle debug interface, E to toggle control point editor";
	if (mIsInSetupMode)
		mStatus += "\nOrigin: "+ofToString(mOriginInstrument)+" "
		+ getName(mOriginInstrument)+", Dest "+ofToString(mDestInstrument)+" "
		+ getName(mDestInstrument)+"\n<num> to change visibility, control <num> to change origin, alt <num> to change dest"
		+ "\n(use '-' for no/all instrument(s)), Ctrl Backspace to clear all control points";
}

string ControlPointEditor::getName(int instrumentNumber) const
{
	if (instrumentNumber<0)
		return "(none)";
	else if (instrumentNumber<NUM_INSTRUMENTS)
		return mRenderer->state().instruments.at(instrumentNumber).name;
	else return "error";
	
}
