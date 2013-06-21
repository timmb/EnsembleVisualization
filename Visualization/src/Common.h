//
//  Common.h
//  Visualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once
#include <string>
#include <vector>
#include "ofMain.h"
#include <iostream>
#include <map>

typedef std::map<int,std::map<int,std::vector<ofVec2f> > > ControlPointMap;

template <typename T>
T sq(T const& x)
{
	return x*x;
}

inline
ofVec2f toNorm(ofVec2f const& v)
{
	return ofVec2f(v.x/ofGetWidth()*2.f-1.f, 1.f-v.y/ofGetHeight()*2.f);
}

inline
ofVec2f toNorm(int x, int y)
{
	return toNorm(ofVec2f(x,y));
}


template <typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> v)
{
	out << "vector: [\n";
	for (T const& t: v)
	{
		out << t << "\n";
	}
	return out << "]";
}