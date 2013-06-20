//
//  OscReceiver.cpp
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "OscReceiver.h"


OscReceiver::OscReceiver()
: mHasNewState(false)
, mTimeListenPortMessageWasLastSent(-42)
, mListenPort(0)
, mHasANewStateEverHappened(false)
, mIsSetup(false)
{
	
}

void OscReceiver::setup(int port, string stabilizerHost, int stabilizerPort)
{
	mOsc.setup(port);
	mSender.setup(stabilizerHost, stabilizerPort);
	mListenPort = port;
	mStabilizerHost = stabilizerHost;
	mStabilizerPort = stabilizerPort;
	mIsSetup = true;
}

void OscReceiver::update(float elapsedTime, float dt)
{
	assert(mIsSetup);
	assert(mState.instruments.size() == NUM_INSTRUMENTS);
	ofxOscMessage m;
	while (mOsc.hasWaitingMessages())
	{
		m.clear();
		mOsc.getNextMessage(&m);
		string address = m.getAddress();
		if (address=="/viz/narrative" && m.getArgType(0)==OFXOSC_TYPE_FLOAT)
		{
			mState.narrative = m.getArgAsFloat(0);
		}
		else if (address=="/viz/note" && m.getArgType(0)==OFXOSC_TYPE_INT32
				 && m.getArgType(1)==OFXOSC_TYPE_FLOAT)
		{
			int inst = m.getArgAsInt32(0);
			if (inst < 0 || inst >= NUM_INSTRUMENTS)
			{
				cout << "ERROR: Note has instrument "<<inst<<" which is out of bounds." << endl;
				continue;
			}
			Note note(elapsedTime, m.getArgAsFloat(1));
			mState.instruments[inst].notes.push_back(note);
		}
		else if (address=="/viz/connections" && m.getArgType(0)==OFXOSC_TYPE_INT32)
		{
			const int num_insts = m.getArgAsInt32(0);
			if (num_insts > NUM_INSTRUMENTS)
			{
				cout << "ERROR: connections message sent with "<<num_insts<<" instruments but we are only setup to work with "<<NUM_INSTRUMENTS<<endl;
				continue;
			}
			else if (num_insts*num_insts + 1 != m.getNumArgs())
			{
				printf("ERROR: connections message declares %d instruments but has %d arguments\n", num_insts, m.getNumArgs());
				continue;
			}
			for (int i=0; i<num_insts; i++)
			{
				for (int j=0; j<num_insts; j++)
				{
					// the + 1 is because the first argument is num_insts
					float v = m.getArgAsFloat(i * num_insts + j + 1);
					mState.instruments.at(i).connections.at(j) = v;
				}
			}
		}
		mHasNewState = true;
		mHasANewStateEverHappened = true;
	}
	mState.update(elapsedTime, dt);
	
	if (elapsedTime - mTimeListenPortMessageWasLastSent > 5)
	{
		ofxOscMessage m;
		m.setAddress("/viz/listen_port");
		m.addIntArg(mListenPort);
		mSender.sendMessage(m);
		mTimeListenPortMessageWasLastSent = elapsedTime;
	}
}

bool OscReceiver::hasNewState() const
{
	return mHasNewState;
}

State OscReceiver::state() const
{
	return mState;
}


/// For debugging
std::string OscReceiver::status() const
{
	
	stringstream ss;
	ss <<"OscReceiver (listen: "<<mListenPort<<" stabilizer: "<<mStabilizerHost<<':'<<mStabilizerPort<<") ";
	ss << (mHasANewStateEverHappened
		? "OSC Data has been received"
		: "Yet to receive OSC data");
	return ss.str();
}