//
//  State.h
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once
#include <vector>
#include <deque>
#include "ofMain.h"
#include <string>
#include <iostream>

#include "Common.h"

static const int NUM_INSTRUMENTS = 8;

struct Note
{
	/// Time a note happened, in terms of elapsed time since the program began
	float time;
	/// Value between 0 and 1
	float intensity;
	
	Note(float time_=0, float intensity_=0)
	: time(time_)
	, intensity(intensity_)
	{}
};
std::ostream& operator<<(std::ostream& out, Note const& note);


struct Instrument
{
	/// normalized coordinate space ([-1,1]x[-1,1])
	ofVec2f pos;
	std::vector<float> connections;
	/// list of recent notes
	std::deque<Note> notes;
	/// Just for debugging, the instruments can be provided with names
	std::string name;
	
	Instrument(ofVec2f const& pos_=ofVec2f())
	: pos(pos_)
	, connections(NUM_INSTRUMENTS)
	{}
	
};
std::ostream& operator<<(std::ostream& out, Instrument const& instrument);


struct State
{
	std::vector<Instrument> instruments;
	float narrative;
	bool debugMode;

	/// Max age of notes that are kept
	static float sMaxNoteAge;
	/// Max number of notes per instrument
	static int sMaxNumNotes;
	
	State();
	void update(float elapsedTime, float dt);
	static State randomState(float elapsedTime);
	/// All connections are 1.
	static State maximalState(float elapsedTime);
};
std::ostream& operator<<(std::ostream& out, State const& state);


	