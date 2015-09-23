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

#include "UI/LayoutController.h"

void clVerticalLayout::LayoutChildViews()
{
	int W = this->GetWidth();
	int H = this->GetHeight();

	int NumFilledViews = 0;

	// 1. Measure
	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( ( Mode & eFillMode_Vertical ) == 0 )
		{
			// view has fixed height, reduce the amount of vertical free space
			H -= i->GetHeight();
		}
		else
		{
			NumFilledViews++;
		}
	}

	// 2. Apply
	int NextPosY = 0;

	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( Mode & eFillMode_Vertical )
		{
			i->SetHeight( H / NumFilledViews );
		}

		if ( Mode & eFillMode_Horizontal )
		{
			i->SetWidth( W - i->GetX() );
		}

		i->SetPosition( i->GetX(), NextPosY );
		NextPosY += i->GetHeight();
	}

	// 3. Process children
	for ( auto& i : this->GetChildViews() )
	{
		i->LayoutChildViews();
	}
}

void clHorizontalLayout::LayoutChildViews()
{
	int W = this->GetWidth();
	int H = this->GetHeight();

	int NumFilledViews = 0;

	// 1. Measure
	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( ( Mode & eFillMode_Horizontal ) == 0 )
		{
			// view has fixed width, reduce the amount of horizontal free space
			W -= i->GetWidth();
		}
		else
		{
			NumFilledViews++;
		}
	}

	// 2. Apply
	int NextPosX = 0;

	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( Mode & eFillMode_Vertical )
		{
			i->SetHeight( H - i->GetY() );
		}

		if ( Mode & eFillMode_Horizontal )
		{
			i->SetWidth( W / NumFilledViews );
		}

		i->SetPosition( NextPosX, i->GetY() );
		NextPosX += i->GetWidth();
	}

	// 3. Process children
	for ( auto& i : this->GetChildViews() )
	{
		i->LayoutChildViews();
	}
}
