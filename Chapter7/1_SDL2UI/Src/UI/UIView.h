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

#pragma once

#include <vector>

#include "IntrusivePtr.h"

class iCanvas;

enum eAlignV
{
	eAlignV_DontCare,
	eAlignV_Top,
	eAlignV_Bottom,
	eAlignV_Center
};

enum eAlignH
{
	eAlignH_DontCare,
	eAlignH_Left,
	eAlignH_Right,
	eAlignH_Center
};

enum eFillMode
{
	eFillMode_Horizontal = 1,
	eFillMode_Vertical   = 2,
};

class clUIView: public iIntrusiveCounter
{
public:
	clUIView();

	virtual void SetPosition( int X, int Y ) { m_X = X; m_Y = Y; }
	virtual void SetSize( int W, int H ) { m_Width = W; m_Height = H; }
	virtual void SetWidth( int W ) { m_Width = W; }
	virtual void SetHeight( int H ) { m_Height = H; }
	virtual int GetWidth() const { return m_Width; }
	virtual int GetHeight() const { return m_Height; }
	virtual int GetX() const { return m_X; }
	virtual int GetY() const { return m_Y; }

	virtual void SetAlignmentV( eAlignV V ) { m_AlignV = V; }
	virtual void SetAlignmentH( eAlignH H ) { m_AlignH = H; }
	virtual eAlignV GetAlignmentV() const { return m_AlignV; }
	virtual eAlignH GetAlignmentH() const { return m_AlignH; }

	virtual void SetParentFractionX( float X ) { m_ParentFractionX = X; }
	virtual void SetParentFractionY( float Y ) { m_ParentFractionY = Y; }

	/// a bit mask of eFillMode flags
	virtual void SetFillMode( int Mode ) { m_FillMode = Mode; }
	virtual int  GetFillMode() const { return m_FillMode; }

	virtual void Draw( const clPtr<iCanvas>& C );

	virtual bool OnTouch( int x, int y, bool Pressed );

	virtual bool IsPointOver( int x, int y ) const;

	virtual void Update( double Delta );

	virtual void Add( const clPtr<clUIView>& V );
	virtual void Remove( const clPtr<clUIView>& V );

	virtual const std::vector< clPtr<clUIView> >& GetChildViews() const { return m_ChildViews; }

	virtual void LayoutChildViews();
	virtual void UpdateScreenPositions( int ParentX = 0, int ParentY = 0 );

	virtual void SetTitle( const std::string& Title ) { m_Title = Title; }
	virtual std::string GetTitle() const { return m_Title; }

	int m_FontHeight;
	int m_FontID;

protected:
	/// these methods should be overriden in most of subclasses to render the actual view (window, button, etc)
	virtual void PreDrawView( const clPtr<iCanvas>& C );
	virtual void PostDrawView( const clPtr<iCanvas>& C ) {};

protected:
	int m_X;
	int m_Y;
	int m_Width;
	int m_Height;
	int m_ScreenX;
	int m_ScreenY;

	std::string m_Title;

private:
	float m_ParentFractionX;
	float m_ParentFractionY;

	eAlignV m_AlignV;
	eAlignH m_AlignH;

	int m_FillMode;

	std::vector< clPtr<clUIView> > m_ChildViews;
};
