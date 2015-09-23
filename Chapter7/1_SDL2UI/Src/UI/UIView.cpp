/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
 * Based on Linderdaum Engine http://www.linderdaum.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov'in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <algorithm>

#include "UIView.h"
#include "Rendering/Canvas.h"
#include "Rendering/TextRenderer.h"

clUIView::clUIView()
	: m_X( 0 )
	, m_Y( 0 )
	, m_Width( 0 )
	, m_Height( 0 )
	, m_ScreenX( 0 )
	, m_ScreenY( 0 )
	, m_ParentFractionX( 1.0f )
	, m_ParentFractionY( 1.0f )
	, m_AlignV( eAlignV_DontCare )
	, m_AlignH( eAlignH_DontCare )
	, m_FillMode( 0 )
	, m_ChildViews( 0 )
	, m_FontHeight( 24 )
	, m_FontID( -1 )
{
}

void clUIView::Draw( const clPtr<iCanvas>& C )
{
	this->PreDrawView( C );

	for ( auto& i : m_ChildViews )
	{
		i->Draw( C );
	}

	this->PostDrawView( C );
}

void clUIView::PreDrawView( const clPtr<iCanvas>& C )
{
	if ( m_FontID < 0 ) { return; }

	if ( !m_Title.empty() )
	{
		clTextRenderer::Instance()->LoadStringWithFont( m_Title, m_FontID, m_FontHeight );

		int MinY, MaxY, Width, BaseLine;
		clTextRenderer::Instance()->CalculateLineParameters( &Width, &MinY, &MaxY, &BaseLine );

		int X1 = m_ScreenX + ( m_Width / 2 - Width / 2 );
		int X2 = m_ScreenX + ( m_Width / 2 + Width / 2 );

		int Y1 = m_ScreenY + ( m_Height / 2 - MinY );
		int Y2 = m_ScreenY + ( m_Height / 2 + MaxY );

		C->TextStr( X1, Y1, X2, Y2, m_Title, m_FontHeight, ivec4( 255, 255, 255, 255 ), m_FontID );
	}
}

void clUIView::LayoutChildViews()
{
	int W = this->GetWidth();
	int H = this->GetHeight();

	for ( auto& i : m_ChildViews )
	{
		if ( i->GetFillMode() & eFillMode_Vertical )
		{
			i->SetHeight( H - i->GetY() );
		}

		if ( i->GetFillMode() & eFillMode_Horizontal )
		{
			i->SetWidth( W );
		}

		// percentage of the parent view
		if ( i->m_ParentFractionX < 1.0f ) { i->SetWidth( static_cast<int>( W * i->m_ParentFractionX ) ); }

		if ( i->m_ParentFractionY < 1.0f ) { i->SetHeight( static_cast<int>( H * i->m_ParentFractionY ) ); }

		switch ( i->GetAlignmentV() )
		{
			case eAlignV_Top:
				i->SetPosition( i->GetX(), 0 );
				break;

			case eAlignV_Bottom:
				i->SetPosition( i->GetX(), H - i->GetHeight() );
				break;

			case eAlignV_Center:
				i->SetPosition( i->GetX(), ( H - i->GetHeight() ) / 2 );
				break;

			case eAlignV_DontCare:
				break;
		}

		switch ( i->GetAlignmentH() )
		{
			case eAlignH_Left:
				i->SetPosition( 0, i->GetY() );
				break;

			case eAlignH_Right:
				i->SetPosition( W - i->GetWidth(), i->GetY() );
				break;

			case eAlignH_Center:
				i->SetPosition( ( W - i->GetWidth() ) / 2, i->GetY() );
				break;

			case eAlignH_DontCare:
				break;
		}

		i->LayoutChildViews();
	}
}

void clUIView::UpdateScreenPositions( int ParentX, int ParentY )
{
	m_ScreenX = ParentX + m_X;
	m_ScreenY = ParentY + m_Y;

	for ( auto& i : m_ChildViews )
	{
		i->UpdateScreenPositions( m_ScreenX, m_ScreenY );
	}
}

void clUIView::Add( const clPtr<clUIView>& V )
{
	m_ChildViews.push_back( V );
}

void clUIView::Remove( const clPtr<clUIView>& V )
{
	m_ChildViews.erase(
	   std::remove( m_ChildViews.begin(), m_ChildViews.end(), V ),
	   m_ChildViews.end()
	);
}

bool clUIView::IsPointOver( int x, int y ) const
{
	return  ( x >= m_ScreenX ) && ( x <= m_ScreenX + m_Width  ) &&
	        ( y >= m_ScreenY ) && ( y <= m_ScreenY + m_Height );
}

bool clUIView::OnTouch( int x, int y, bool Pressed )
{
	if ( IsPointOver( x, y ) )
	{
		// check if on of sub-views handles this event
		for ( auto& i : m_ChildViews )
			if ( i->OnTouch( x, y, Pressed ) )
			{
				return true;
			}
	}

	return false;
}

void clUIView::Update( double Delta )
{
	for ( auto& i : m_ChildViews )
	{
		i->Update( Delta );
	}
}
