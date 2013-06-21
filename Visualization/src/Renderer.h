//
//  Renderer.h
//  EnsembleVisualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#pragma once

#include "State.h"
#include "Common.h"


class Renderer
{
public:
	Renderer();
	virtual ~Renderer();
	
	void setState(State const& newState);
	State state() const;

	void draw(float elapsedTime, float dt);
	
	/// control points for splines
	void setControlPoints(ControlPointMap const& points);
	ControlPointMap controlPoints() const;
	
	void setEnableDrawConnectionsDebug(bool enabled);
	bool isDrawConnectionsDebugEnabled() const { return mEnableDrawConnectionsDebug; }

private:
	void drawQuad(ofVec2f const& pos, ofVec2f const& size);
	void drawConnectionsDebug();
//	ofVec2f interp(ofVec2f const& orig, ofVec2f const& dest, float t);
//	// optimization of above
//	ofVec2f interp(int inst0, int inst1, float t);
	bool mEnableDrawConnectionsDebug;
	State mState;
	ofTexture mParticleTex;
	
	/// just control points
	std::map<int,std::map<int,std::vector<ofVec2f> > > mControlPoints;
	/// including start and end points too
	std::map<int,std::map<int,std::vector<ofVec2f> > > mCalculatedControlPoints;
	void updateCalculatedControlPoints();
	
	float mx, my;
	
	template <typename T>
	static T hermiteSpline(T const& point0, T const& tangent0, T const& point1, T const& tangent1, float t);
	
	template <typename T>
	static T hermiteSpline(std::vector<T> const& points, float t);
	
	ofVec2f interpHermite(int inst0, int inst1, float t) const;
	
};

//template<typename T, typename L>
//T bezierInterp( const T &a, const T &b, const T &c, const T &d, L t)
//{
//    L t1 = static_cast<L>(1.0) - t;
//    return a*(t1*t1*t1) + b*(3*t*t1*t1) + c*(3*t*t*t1) + d*(t*t*t);
//}
