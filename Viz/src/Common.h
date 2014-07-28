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
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include <iostream>
#include <ios>
#include <map>
#include <sstream>


typedef std::map<int,std::map<int,std::vector<ci::Vec2f> > > ControlPointMap;

template <typename T>
T sq(T const& x)
{
	return x*x;
}

const static double PI = 3.14159265359;
/// maximum number of control points per particle
/// This value is used directly in the shader as well
const static int MAX_CONTROL_POINTS = 50;

//inline
//ofVec2f toNorm(ofVec2f const& v)
//{
//	return ofVec2f(v.x/ofGetWidth()*2.f-1.f, 1.f-v.y/ofGetHeight()*2.f);
//}

inline
ci::Vec2f toNorm(ci::Vec2f const& v)
{
	using namespace ci;
	return Vec2f(v.x/app::getWindowWidth()*2.f-1.f, 1.f-v.y/app::getWindowHeight()*2.f);
}

inline
ci::Vec2f toNorm(int x, int y)
{
	return toNorm(ci::Vec2f(x,y));
}


template <typename T>
std::ostream& operator<<(std::ostream& outstream, std::vector<T> v)
{
	outstream << "vector: [\n";
	//for (T const& t: v)
	for (std::vector<T>::const_iterator vItr = v.begin(); vItr != v.end(); ++vItr)
	{
		const T & t = *vItr;

		outstream << t << '\n';
	}
	return outstream << ']';
}

/// mouse x normalized
extern float mx;
/// mouse y normalized
extern float my;
namespace tmb
{
	void drawString(std::string const& text, ci::Vec2f const& position, bool isCentered = true, ci::ColorA const& color=ci::ColorA(1, .6, .45, 1.));
	
	struct Quad {
		ci::Vec2f tl, tr, br, bl;
		
		Quad(ci::Vec2f const& tl_, ci::Vec2f const& tr_, ci::Vec2f const& br_, ci::Vec2f const& bl_)
		: tl(tl_)
		, tr(tr_)
		, br(br_)
		, bl(bl_)
		{}
		
		Quad()
		: tl(-1, 1)
		, tr(1, 1)
		, br(1, -1)
		, bl(-1, -1)
		{}
	};
	
	std::ostream& operator<<(std::ostream& outstream, Quad quad);
}
	
namespace cinder
{
	template <typename T>
	std::istream& operator>>( std::istream& lhs, Vec2<T>& rhs )
	{
		Vec2f v;
		char c = '\0';
		if (!(lhs >> c) || c!='[')
		{
			lhs.setstate(std::ios_base::failbit);
		}
		if (lhs)
			lhs >> v.x;
		if (lhs && (!(lhs>>c) || c!=','))
			lhs.setstate(std::ios_base::failbit);
		if (lhs)
			lhs >> v.y;
		if (lhs && (!(lhs>>c) || c!=']'))
			lhs.setstate(std::ios_base::failbit);
		if (lhs)
			rhs = v;
		return lhs;
	}
	
	template <typename T>
	bool operator>>(std::string const& lhs, Vec2<T>& rhs)
	{
		std::istringstream ss(lhs);
		ss >> rhs;
		return ss.good();
	}
}
