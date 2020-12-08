#include "MultiTouchTest.h"

#include "cinder/Log.h"
#include "cinder/gl/draw.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 40.0f;

//const size_t NUM_DRAGGABLES = 100;
//const size_t NUM_TOUCHES = 1;

const size_t NUM_DRAGGABLES = 1000;
const size_t NUM_TOUCHES = 32;

//const size_t NUM_DRAGGABLES = 5000;
//const size_t NUM_TOUCHES = 64;

class DraggableView : public vu::RectView {
  public:
	DraggableView()
			: RectView( Rectf::zero() )
	{
		setColor( mDefaultColor );
	}
  protected:
	bool touchesBegan( app::TouchEvent &event )	override
	{
		setColor( mBeganColor );

		auto &firstTouch = event.getTouches().front();
		mPrevPos = firstTouch.getPos(); // Don't need local position as we'll just use the difference

		firstTouch.setHandled();
		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		setColor( mDragColor );

		auto &firstTouch = event.getTouches().front();
		vec2 pos = firstTouch.getPos();

		vec2 diff = pos - mPrevPos;
		mPrevPos = pos;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		setColor( mDefaultColor );

		auto &firstTouch = event.getTouches().front();
		vec2 pos = firstTouch.getPos();

		vec2 diff = pos - mPrevPos;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	Color mBeganColor = { 1, 0.7f, 0 };
	Color mDragColor = { 0.8f, 0.4f, 0 };
	Color mDefaultColor = { 0, 0.2f, 0.6f };

	vec2 mPrevPos; // TODO: pass this through in test injection and pull out of Touch
};

MultiTouchTest::MultiTouchTest()
		: SuiteView()
{
	mTestTouches.resize( NUM_TOUCHES );

	setupControls();
	setupDraggables();

	mDraggablesContainer->setHidden();
}

void MultiTouchTest::setupControls()
{
	mControlsContainer = make_shared<vu::View>();
	mControlsContainer->setLabel( "controls container" );
	mControlsContainer->setFillParentEnabled();

	mButton = make_shared<vu::Button>();
	mButton->setTitle( "Moe" );
	mButton->getSignalPressed().connect( [] { CI_LOG_V( "Bob pressed" ); } );
	mButton->getSignalReleased().connect( [] { CI_LOG_V( "Bob released" ); } );

	mToggle = make_shared<vu::Button>();
	mToggle->setAsToggle();
	mToggle->setLabel( "toggle" );
	mToggle->setTitle( "Larry" );
	mToggle->setTitle( "Curly", vu::Button::State::ENABLED );
	mToggle->setColor( Color( 0.2f, 0.5f, 0.5f ), vu::Button::State::ENABLED );
	mToggle->getSignalPressed().connect( [] { CI_LOG_V( "toggle pressed" ); } );
	mToggle->getSignalReleased().connect( [] { CI_LOG_V( "toggle released" ); } );

	mVSlider1 = make_shared<vu::VSlider>();
	mVSlider1->setLabel( "slider1" );
	mVSlider1->getBackground()->setColor( ColorA( "green", 0.5f ) );

	// temp - adding constrols to this test
	mVSlider2 = make_shared<vu::VSlider>();
	mVSlider2->setLabel( "slider2" );
//	mVSlider2->setCancelPadding( Rectf( 40, 40, 200, 40 ) );
	mVSlider2->getBackground()->setColor( ColorA( "green", 0.5f ) );

	mControlsContainer->addSubviews( { mButton, mToggle, mVSlider1, mVSlider2 } );
	addSubview( mControlsContainer );
}

void MultiTouchTest::setupDraggables()
{
	mDraggablesContainer = make_shared<vu::View>();
	mDraggablesContainer->setLabel( "draggables container" );
	mDraggablesContainer->setFillParentEnabled();

	for( size_t i = 0; i < NUM_DRAGGABLES; i++ ) {
		auto draggable = make_shared<DraggableView>();
		draggable->setLabel( "DraggableView-" + to_string( i ) );
		draggable->setSize( vec2( 60 ) );
		mDraggablesContainer->addSubview( draggable );
	}

	addSubview( mDraggablesContainer );
}

void MultiTouchTest::layoutControls()
{
	Rectf buttonBounds( PADDING, PADDING, PADDING + 120, PADDING + 60 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	Rectf sliderBounds = Rectf( PADDING, buttonBounds.y2 + PADDING, PADDING + 120, buttonBounds.y2 + 300 + PADDING );
	mVSlider1->setBounds( sliderBounds );

	sliderBounds += vec2( sliderBounds.getWidth() + PADDING, 0 );
	mVSlider2->setBounds( sliderBounds );
}

void MultiTouchTest::layoutDraggables()
{
	// TODO: mDraggablesContainer's setFillParentEnabled() hasn't been done yet, so can't rely on it's size.
	// - should it before resized before View::layout() is called? It is a subview.
//	auto containerSize = mDraggablesContainer->getSize();
	auto containerSize = getSize();
	for( auto &draggable : mDraggablesContainer->getSubviews() ) {
		draggable->setPos( vec2( randFloat( PADDING, containerSize.x - PADDING ), randFloat( PADDING, containerSize.y - PADDING ) ) );
	}
}

void MultiTouchTest::layout()
{
	layoutControls();
	layoutDraggables();

//	injectTouches();
}

bool MultiTouchTest::keyDown( app::KeyEvent &event )
{
	if( event.isControlDown() )
		return false;

	bool handled = true;
	switch( event.getChar() ) {
		case 'v':
			CI_LOG_I( "num views with touches: " << getGraph()->getViewsWithTouches().size() );
		break;
		case 'a': {
			const auto &allTouches = getGraph()->getAllTouchesInWindow();
			CI_LOG_I( "num touches in window: " << allTouches.size() );
		}
		break;
		case '1':
			mControlsContainer->setHidden( false );
			mDraggablesContainer->setHidden( true );
		break;
		case '2':
			mControlsContainer->setHidden( true );
			mDraggablesContainer->setHidden( false );
		break;
		case 'c': {
			CI_LOG_I( "this: " << hex << this << dec );

			mEnableContinuousInjection = ! mEnableContinuousInjection;
			CI_LOG_I( "mEnableContinuousInjection: " << mEnableContinuousInjection );
			if( ! mEnableContinuousInjection )
				endContinuousTouches();

			// avoid hitting the test selector and inadvertently changing tests
			// - comment this out to test deleting Views while iterating over them
			auto testSelector = getSuite()->getSelector();
			testSelector->setInteractive( ! mEnableContinuousInjection );
		}
		break;
		default:
			handled = false;
	}

	return handled;
}

void MultiTouchTest::injectTouches()
{
	vec2 pos1 = mVSlider1->getCenter();
	vec2 pos2 = mVSlider2->getCenter();

	vector<app::TouchEvent::Touch> touches = {
			app::TouchEvent::Touch( pos1, vec2( 0 ), 1, 0, nullptr ),
			app::TouchEvent::Touch( pos2, vec2( 0 ), 2, 0, nullptr )
	};

	auto graph = getGraph();
	{
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesBegan( touchEvent );
	}

	{
		pos1 += vec2( 0, 60 );
		pos2 += vec2( 0, 60 );
		touches.at( 0 ).setPos( pos1 );
		touches.at( 1 ).setPos( pos2 );
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesMoved( touchEvent );
	}

	{
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesEnded( touchEvent );
	}
}

void MultiTouchTest::injectContinuousTouches()
{
	const float maxDragDistance = 100;
	const vec2 dragDurationRange = { 0.5f, 2.0f };
	const vec2 containerSize = mDraggablesContainer->getSize();

	for( size_t i = 0; i < mTestTouches.size(); i++ ) {
		if( mTestTouches[i].mPhase == TestTouch::Phase::UNUSED ) {
			vec2 startPos( randFloat( PADDING, containerSize.x - PADDING ), randFloat( PADDING, containerSize.y - PADDING ) );
			vec2 endPos( randFloat( startPos.x - maxDragDistance, startPos.x + maxDragDistance ), randFloat( startPos.y - maxDragDistance, startPos.y + maxDragDistance ) );
			endPos = min( endPos, containerSize - vec2( PADDING ) );

			float duration = randFloat( dragDurationRange[0], dragDurationRange[1] );

			//CI_LOG_I( "cueing test touch " << i << ", startPos: " << startPos << ", endPos: " << endPos << ", duration: " << duration );

			mTestTouches[i].mPhase = TestTouch::Phase::BEGAN;

			mTestTouches[i].mPos = startPos;
			app::timeline().apply( &mTestTouches[i].mPos, endPos, duration )
					.easeFn( EaseInOutQuad() )
					.updateFn( [this, i] {
						mTestTouches[i].mPhase = TestTouch::Phase::MOVED;
					} )
					.finishFn( [this, i] {
						mTestTouches[i].mPhase = TestTouch::Phase::ENDED;
					} );
		}
	}

	// send off all touches at once
	auto graph = getGraph();
	vector<app::TouchEvent::Touch> touchesBegan;
	vector<app::TouchEvent::Touch> touchesMoved;
	vector<app::TouchEvent::Touch> touchesEnded;

	for( uint32_t i = 0; i < mTestTouches.size(); i++ ) {
		auto &t = mTestTouches[i];
		if( t.mPhase == TestTouch::Phase::BEGAN )
			touchesBegan.emplace_back( t.mPos, vec2( 0 ), i, 0.0, nullptr );
		else if( t.mPhase == TestTouch::Phase::MOVED )
			touchesMoved.emplace_back( t.mPos, vec2( 0 ), i, 0.0, nullptr );
		else if( t.mPhase == TestTouch::Phase::ENDED ) {
			touchesEnded.emplace_back( t.mPos, vec2( 0 ), i, 0.0, nullptr );
			t.mPhase = TestTouch::Phase::UNUSED;
		}
	}

	if( ! touchesBegan.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesBegan );
		graph->propagateTouchesBegan( touchEvent );
	}
	if( ! touchesMoved.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesMoved );
		graph->propagateTouchesMoved( touchEvent );
	}
	if( ! touchesEnded.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesEnded );
		graph->propagateTouchesEnded( touchEvent );
	}
}

void MultiTouchTest::endContinuousTouches()
{
	vector<app::TouchEvent::Touch> touchesEnded;
	for( uint32_t i = 0; i < mTestTouches.size(); i++ ) {
		auto &t = mTestTouches[i];
		if( t.mPhase == TestTouch::Phase::UNUSED )
			continue;

		touchesEnded.emplace_back( t.mPos, vec2( 0 ), i, 0.0, nullptr );
		t.mPhase = TestTouch::Phase::UNUSED;
		t.mPos.stop();
	}

	auto graph = getGraph();
	app::TouchEvent touchEvent( graph->getWindow(), touchesEnded );
	graph->propagateTouchesEnded( touchEvent );
}

void MultiTouchTest::update()
{
	if( mEnableContinuousInjection && ! mDraggablesContainer->isHidden() ) {
		injectContinuousTouches();
	}
}
