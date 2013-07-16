#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "Common.h"
#include "Renderer.h"
#include "State.h"
#include "OscReceiver.h"
#include "ControlPointEditor.h"

using namespace std;

class VizApp : public ci::app::AppNative {
  public:
	VizApp();
	void prepareSettings(Settings* settings);
	void setup();
	void mouseDown( ci::app::MouseEvent event );
	void mouseMove(ci::app::MouseEvent event);
	void mouseDrag(ci::app::MouseEvent event);
	void mouseUp(ci::app::MouseEvent event);
	void keyDown(ci::app::KeyEvent event);
	void keyUp(ci::app::KeyEvent event);
	void update();
	void draw();
	
	
private:
	int mListenPort;
	std::string mStabilizerHost;
	int mStabilizerPort;
	
	float mElapsedTime;
	float mDt;
	Renderer* mRenderer;
	OscReceiver mOscReceiver;
	ControlPointEditor mEditor;
	
	// for setting the control points. only shown on debug
	std::string getName(int instrumentNumber);
	/// which instrument control points are visible
	vector<bool> mInstrumentVisibility;
	/// being currently edited - set to -1 for none
	int mCurrentOrig;
	int mCurrentDest;
	bool mPrintFrameRate;
};

VizApp::VizApp()
: mElapsedTime(-1.)
, mDt(1.)
, mStabilizerHost("127.0.0.1")
, mStabilizerPort(1123)
, mListenPort(12378)
, mRenderer(NULL)
, mInstrumentVisibility(NUM_INSTRUMENTS, true)
, mCurrentOrig(-1)
, mCurrentDest(-1)
, mPrintFrameRate(false)
{
}

void VizApp::prepareSettings(Settings *settings)
{
	settings->setWindowSize(700, 700);
}

void VizApp::setup()
{
	mRenderer = new Renderer;
	mRenderer->setState(State::randomState(0));
	mOscReceiver.setup(mListenPort, mStabilizerHost, mStabilizerPort);
	mEditor.setup(mRenderer);
}

void VizApp::mouseDown( ci::app::MouseEvent event )
{
	using namespace ci;
	using namespace ci::app;
	Vec2f pos = Vec2f(float(event.getPos().x)/getWindowWidth() * 2 - 1, float(getWindowWidth() - event.getPos().y)/getWindowHeight() * 2 - 1);
	
	// legacy oF stuff
	static const int LEFT = 0;
	static const int RIGHT = 2;
	int button = -1;
	if (event.isLeft())
		button = LEFT;
	else if (event.isMiddle())
		button = 1;
	else if (event.isRight())
		button = RIGHT;
	
	mEditor.mousePressed(pos, button);
}

void VizApp::mouseMove(ci::app::MouseEvent event)
{
	mx = event.getPos().x / (float)getWindowWidth();
	my = event.getPos().y / (float)getWindowHeight();
}

void VizApp::mouseDrag(ci::app::MouseEvent event)
{
	mouseMove(event);

}

void VizApp::mouseUp(ci::app::MouseEvent event)
{
	
}

void VizApp::keyDown(ci::app::KeyEvent event)
{
	using namespace ci;
	using namespace ci::app;
	char key = event.getChar();
//	bool ctrl = glutGetModifiers() & GLUT_ACTIVE_CTRL;
//	bool alt = glutGetModifiers() & GLUT_ACTIVE_ALT;
	
	if (key=='r')
	{
		bool d = mOscReceiver.state().debugMode;
		State state = State::randomState(getElapsedSeconds());
		state.debugMode = d;
		mOscReceiver.setState(state);
	}
	else if (key=='m')
	{
		bool d = mOscReceiver.state().debugMode;
		State state = State::maximalState(getElapsedSeconds());
		state.debugMode = d;
		mOscReceiver.setState(state);
	}
	else if (key=='s')
		mEditor.save();
	else if (key=='l')
		mEditor.load();
	else if (key==' ')
		mOscReceiver.toggleDebugMode();
	else if (key=='p')
		std::cout << "Renderer state:\n"<<mRenderer->state()<<endl;
	else if (key=='a')
		mRenderer->loadShader();
	else if (key=='f')
		mPrintFrameRate = !mPrintFrameRate;
	mEditor.keyPressed(event);

}

void VizApp::keyUp(ci::app::KeyEvent event)
{
	
}

std::string VizApp::getName(int instrumentNumber)
{
	if (instrumentNumber==-1)
		return "(none)";
	else if (instrumentNumber<NUM_INSTRUMENTS)
		return mOscReceiver.state().instruments.at(instrumentNumber).name;
	else return "error";
}

void VizApp::update()
{
	using namespace ci;
	using namespace ci::app;

	float t = getElapsedSeconds();
	float dt = t - mElapsedTime;
	mElapsedTime = t;
	mOscReceiver.update(mElapsedTime, dt);
	if (mOscReceiver.hasNewState())
	{
		mRenderer->setState(mOscReceiver.state());
	}
	// debugging
	if (0)
	{
		if (getElapsedFrames()%30==0)
		{
			cout << mOscReceiver.status() << endl;
			cout << "State at time "<<mElapsedTime<<"\n"
			<< mOscReceiver.state() << endl;
		}
		cout << 1.f/dt << endl;
	}
	if (mPrintFrameRate)
	{
		cout << "dt: " <<dt << "\nfps: " << 1.f/dt << endl;
	}
}

void VizApp::draw()
{
	// clear out the window with black
	ci::gl::clear( ci::Color( 0, 0, 0 ) );
	mRenderer->draw(mElapsedTime, mDt);
	mEditor.draw(mElapsedTime, mDt);
}

CINDER_APP_NATIVE( VizApp, ci::app::RendererGl )
