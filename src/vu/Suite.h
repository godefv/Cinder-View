/*
 Copyright (c) 2015, Richard Eakin - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided
 that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "vu/Graph.h"
#include "vu/Control.h"

#include "mason/Factory.h"

namespace vu {

typedef std::shared_ptr<class Suite>		SuiteRef;
typedef std::shared_ptr<class SuiteView>	SuiteViewRef;

class Suite;

//! View type that can be registered with and managed by Suite.
class CI_UI_API SuiteView : public vu::View {
  public:
	SuiteView( const ci::Rectf &bounds = ci::Rectf::zero() );

	Suite*  getSuite() const   { return mSuite; }

  private:
	Suite*  mSuite = nullptr;

	friend class Suite;
};

//! Class for managing a selection of Views with some basic Controls and automatic layout. Useful for test or sample sets.
class CI_UI_API Suite {
  public:
	Suite( const vu::Graph::EventOptions &eventOptions = {} );

	//! Registers a subclass of SuiteView with an associated \a key that will be displayed in a VSelector on screen.
	template<typename Y>
	void registerSuiteView( const std::string &key );

	//! Selects a registered SuiteView by index
	void                select( size_t index );
	//! Returns the index of the currently selected SuiteView
	size_t              getCurrentIndex() const	    { return mSelector->getSelectedIndex(); }
	//! Returns the key associated with the currently selected SuiteView

	const std::string&  getCurrentKey() const   { return mCurrentTestKey; }
	//! Returns the Control used for selecting a SuiteView
	vu::VSelectorRef	getSelector() const	        { return mSelector; }
	//! Signal that is emitted before the current SuiteView changes.
	ci::signals::Signal<void ()>&	getSignalSuiteViewWillChange()	{ return mSignalSuiteViewWillChange; }
	//! Signal that is emitted after the current SuiteView has changed.
	ci::signals::Signal<void ()>&	getSignalSuiteViewDidChange()	{ return mSignalSuiteViewDidChange; }
	//! Reloads the current SuiteView
	void reload();

	//! Causes the Graph to be updated
	void update();
	//! Causes the Graph to be drawn
	void draw();
	//! Returns the Graph owned by this Suite
	vu::GraphRef	getGraph() const	{ return mGraph; }

	void setDrawUiEnabled( bool enable );
	bool isDrawUiEnabled() const	{ return mDrawUi; }

  private:
	void resize();
	void selectTest( const std::string &key );

	vu::GraphRef		mGraph;
	SuiteViewRef		mCurrentSuiteView;
	vu::VSelectorRef	mSelector;
	std::string			mCurrentTestKey;
	bool				mDrawUi = true;

	mason::Factory<SuiteView>		mFactory;
	ci::signals::Signal<void ()>	mSignalSuiteViewWillChange, mSignalSuiteViewDidChange;
};

template<typename Y>
void Suite::registerSuiteView( const std::string &key )
{
	mFactory.registerBuilder<Y>( key );
	mSelector->getSegmentLabels().push_back( key );
}

} // namespace vu
