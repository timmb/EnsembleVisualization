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

void ControlPointEditor::setup(Renderer* renderer)
{
	mJsonFilename = ofToDataPath("control_points.json");
	mRenderer = renderer;
	load();
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
}