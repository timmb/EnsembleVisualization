//
//  State.cpp
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "State.h"
#include <iomanip>
#include "cinder/Rand.h"

using namespace std;
using namespace ci;


float State::sMaxNoteAge = 600.f;
int State::sMaxNumNotes = 500;


State::State()
: narrative(0.)
, instruments(NUM_INSTRUMENTS)
, debugMode(false)
{
	for (int i=0; i<NUM_INSTRUMENTS; ++i)
	{
		float theta = float(i)/NUM_INSTRUMENTS * 2*PI;
		instruments.at(i) = Instrument(Vec2f(cos(theta), sin(theta)));
	}
}


void State::update(float elapsedTime, float dt)
{
	/// Important to prevent a potential bounds error in the while condition
	sMaxNumNotes = max(0, sMaxNumNotes);
	for (int i=0; i<instruments.size(); ++i)
	{
		deque<Note>& notes = instruments[i].notes;
		while (notes.size()>sMaxNumNotes
			   || (!notes.empty() && elapsedTime - notes.front().time > sMaxNoteAge))
		{
			notes.pop_front();
		}
	}
}


State State::randomState(float elapsedTime)
{
	Rand::randomize();
	State state;
	for (int i=0; i<NUM_INSTRUMENTS; i++)
	{
		for (int j=0; j<=i; j++)
		{
			float r = Rand::randFloat(1);
			r *= r;
			if (i==j) r=1;
			state.instruments.at(i).connections.at(j) = r;
			state.instruments.at(j).connections.at(i) = r;
		}
		float r = Rand::randFloat(0.9999);
		r *= r*r;
		int n = int(5*r);
		for (int i=0; i<n; i++)
		{
			float t = Rand::randFloat(30);
			t *= Rand::randFloat(1);
			Note note(elapsedTime-t, Rand::randFloat(1.));
			state.instruments.at(i).notes.push_back(note);
		}
	}
	std::cout << "Created random state:\n"<<state<<std::endl;
	return state;
}

State State::maximalState(float elapsedTime)
{
	State state;
	for (int i=0; i<NUM_INSTRUMENTS; i++)
		for (int j=0; j<NUM_INSTRUMENTS; j++)
		{
			state.instruments.at(i).connections.at(j) = 1.f;
		}
	return state;
}



std::ostream& operator<<(std::ostream& out, Note const& note)
{
	return out << "Note(t:"<<note.time<<",i:"<<note.intensity<<')';
}

std::ostream& operator<<(ostream& out, Instrument const& inst)
{
	out << std::setprecision(2);
	out << "Instrument(pos:"<<inst.pos<<", conn:";
	for (int i=0;i<inst.connections.size(); ++i)
	{
		out<<i<<"/"<<inst.connections.at(i);
		if (i<inst.connections.size()-1)
			out << ", ";
	}
	out << " notes:";
	for (int i=0; i<inst.notes.size(); ++i)
	{
		out << inst.notes.at(i);
		if (i<inst.notes.size()-1)
			out << ", ";
	}
	out << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, State const& state)
{
	out << "State(narr:"<<state.narrative<<",instruments:\n";
	for (int i=0; i<state.instruments.size(); ++i)
	{
		out << state.instruments.at(i) << '\n';
	}
	out << ')';
	return out;
}

