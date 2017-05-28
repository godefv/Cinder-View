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

#include "ui/Interface3d.h"
#include "ui/Graph.h"

#include "cinder/gl/gl.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;

namespace ui {

// ----------------------------------------------------------------------------------------------------
// Interface3dBaseView
// ----------------------------------------------------------------------------------------------------

void Interface3dBaseView::layout()
{
	vec2 size = getSize();
	if( size.x < 1.0f || size.y < 1.0f )
		return;

	mCam.setPerspective( 40, size.x / size.y, 0.05f, 100.0f );
	mCam.lookAt( vec3( 0 ), mPlacement );
}

void Interface3dBaseView::draw( Renderer *ren )
{
	Rectf worldBounds = getWorldBounds();
	ivec2 pos = worldBounds.getLowerLeft();

	// TODO: using clipping size here because it works, but sort of indicates the method is misnamed
	pos.y = getGraph()->getClippingSize().y - pos.y; // flip y relative to window's bottom left
	ivec2 size = getSize();
	if( size.x == 0 || size.y == 0 )
		return;

	gl::ScopedViewport scopedViewport( pos, size );
	draw3d();
}

// ----------------------------------------------------------------------------------------------------
// CoordinateAxisView
// ----------------------------------------------------------------------------------------------------

CoordinateAxisView::CoordinateAxisView( const ci::Rectf &bounds )
	: Interface3dBaseView( bounds )
{
}

void CoordinateAxisView::layout()
{
	Interface3dBaseView::layout();

	const float rayPadding = 0.27f;

	if( getWidth() > getHeight() ) {
		vec3 topPos = mCam.generateRay( 0, rayPadding, mCam.getAspectRatio() ).calcPosition( mPlacement.z );
		vec3 bottomPos = mCam.generateRay( 0, 1 - rayPadding, mCam.getAspectRatio() ).calcPosition( mPlacement.z );
		mArrowLength = topPos.y - bottomPos.y;
	}
	else {
		vec3 rightPos = mCam.generateRay( 1 - rayPadding, 0, mCam.getAspectRatio() ).calcPosition( mPlacement.z );
		vec3 leftPos = mCam.generateRay( rayPadding, 0, mCam.getAspectRatio() ).calcPosition( mPlacement.z );
		mArrowLength = leftPos.x - rightPos.x;
	}
}

void CoordinateAxisView::draw3d()
{
	gl::ScopedGlslProg glslScope( gl::getStockShader( gl::ShaderDef().lambert().color() ) );
	gl::ScopedDepth depthScope( true );
	gl::ScopedMatrices matricesScope;

	gl::setMatrices( mCam );
	gl::translate( mPlacement );
	gl::multModelMatrix( mat4( inverse( mOrientation ) ) );

	float headRadius = mArrowLength * 0.114f;
	float headLength = mArrowLength * 0.13f;
	gl::drawCoordinateFrame( mArrowLength, headLength, headRadius );
}

} // namespace ui
