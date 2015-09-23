/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
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

#include <stdio.h>
#include <SDL2/SDL.h>

#include "Platform.h"
#include "SDLLibrary.h"
#include "SDLWindow.h"
#include "Rendering/Canvas.h"
#include "Rendering/TextRenderer.h"

//#include "UI/UIStatic.h"
#include "UI/UIButton.h"
#include "UI/LayoutController.h"

#include "FI_Utils.h"

#include "FileSystem.h"
#include "Archive.h"
#include "Log.h"
#include "Bitmap.h"

#include "LGL/LGL.h"

#if defined(OS_WINDOWS)
#  undef main
#endif // OS_WINDOWS

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clFileSystem> g_FS;

clPtr<clUIView> UIRoot = nullptr;

void OnStart( const std::string& RootPath )
{
	LOGI( "Mounting %s", RootPath.c_str() );

	g_FS = make_intrusive<clFileSystem>();

	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );
}

bool FreeImage_SaveBitmapToFile( const std::string& FileName, uint8_t* ImageData, int W, int H, int BitsPP );

clPtr<clSDLWindow> g_Window = nullptr;

void OnMove( int X, int Y )
{
}

void OnKey( int KeyCode, bool Pressed )
{
	g_Window->RequestExit();
}

// OnTouch method redirects
void OnTouch( int mx, int my, bool Touch )
{
	UIRoot->OnTouch( mx, my, Touch );
}

int main( int argc, char** argv )
{
	clSDL SDLLibrary;

	LoadFreeImage();

	g_Window = clSDLWindow::CreateSDLWindow( "SDL2UI", 1024, 768 );

	auto Canvas = make_intrusive<clGLCanvas>( g_Window );

	UIRoot = make_intrusive<clUIView>();

	g_Window->SetRootView( UIRoot );

#if !defined(ANDROID)
	OnStart( "." );
#endif


//	auto Stream = g_FS->CreateReader("test.bmp");
//	auto bmp = clBitmap::LoadImg(Stream);
//	int txt = Canvas->CreateTexture( bmp->GetWidth(), bmp->GetHeight(), bmp->FBitmapData );
//	FreeImage_SaveBitmapToFile( "test_out.bmp", bmp->FBitmapData, bmp->GetWidth(), bmp->GetHeight(), 24 );

	/*
	   auto TextBitmap = clTextRenderer::Instance()->RenderTextWithFont( "Hello World!", FontHandle, 64, ivec4(255, 255, 255, 0), true );

	   if ( !TextBitmap )
	   {
	      LOGI("No TextBitmap, check the location of receipt.ttf");

	      return 0;
	   }

	   int TextTexture = Canvas->CreateTexture( TextBitmap );

	   FreeImage_SaveBitmapToFile( "test_txt.bmp", TextBitmap->FBitmapData, TextBitmap->GetWidth(), TextBitmap->GetHeight(), TextBitmap->FBitmapParams.GetBitsPerPixel() );
	*/

	// construct a simple demo UI

	/*
	   UIRoot
	      |
	      |- Background
	      |- VerticalLayout
	         |
	         |- TopBar
	         |- HorizontalLayout
	         |     |- Panel1
	         |     |- Panel2
	         |
	         |- CommandLine
	         |- BottomBar
	      |
	      |- MsgBox

	*/
	auto Background = make_intrusive<clUIStatic>();
	Background->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	Background->SetBackgroundColor( ivec4( 0 ) );

	auto VerticalLayout = make_intrusive<clVerticalLayout>();
	VerticalLayout->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	auto HorizontalLayout = make_intrusive<clHorizontalLayout>();
	HorizontalLayout->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );

	auto Panel1 = make_intrusive<clUIStatic>();
	Panel1->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	Panel1->SetBackgroundColor( ivec4( 0, 0, 255, 255 ) );
	auto Panel2 = make_intrusive<clUIStatic>();
	Panel2->SetBackgroundColor( ivec4( 0, 0, 128, 255 ) );
	Panel2->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	HorizontalLayout->Add( Panel1 );
	HorizontalLayout->Add( Panel2 );

	auto TopBar = make_intrusive<clUIStatic>();
	TopBar->SetFillMode( eFillMode_Horizontal );
	TopBar->SetHeight( 50 );
	TopBar->SetBackgroundColor( ivec4( 255, 0, 0, 255 ) );
	VerticalLayout->Add( TopBar );
	VerticalLayout->Add( HorizontalLayout );
	auto CommandLine = make_intrusive<clUIStatic>();
	CommandLine->SetFillMode( eFillMode_Horizontal );
	CommandLine->SetHeight( 50 );
	CommandLine->SetBackgroundColor( ivec4( 0, 255, 0, 255 ) );
	VerticalLayout->Add( CommandLine );
	auto BottomBar = make_intrusive<clUIStatic>();
	BottomBar->SetFillMode( eFillMode_Horizontal );
	BottomBar->SetHeight( 50 );
	BottomBar->SetBackgroundColor( ivec4( 255, 255, 0, 255 ) );
	VerticalLayout->Add( BottomBar );

	auto ExitBtn = make_intrusive<clUIButton>();
	ExitBtn->m_FontID = clTextRenderer::Instance()->GetFontHandle( "receipt.ttf" );
	ExitBtn->m_FontHeight = 24;
	ExitBtn->SetParentFractionX( 0.5f );
	ExitBtn->SetParentFractionY( 0.5f );
	ExitBtn->SetAlignmentV( eAlignV_Center );
	ExitBtn->SetAlignmentH( eAlignH_Center );
	ExitBtn->SetBackgroundColor( ivec4(  255, 255, 255, 255 ) );
	ExitBtn->SetTitle( "Exit" );

	ExitBtn->SetTouchHandler(
	   []( int x, int y )
	{
		LOGI( "Exiting" );

		g_Window->RequestExit();
		return true;
	}
	);

	UIRoot->Add( Background );
	UIRoot->Add( VerticalLayout );
	UIRoot->Add( ExitBtn );
	//

	while ( g_Window && g_Window->HandleInput() )
	{
		Canvas->SetColor( 0, 0, 0 );
		Canvas->Clear();

		UIRoot->UpdateScreenPositions();
		UIRoot->Draw( Canvas );

		Canvas->SetColor( 255, 255, 255 );

		Canvas->Present();
	}

	Canvas = nullptr;
	g_Window = nullptr;

	return 0;
}
