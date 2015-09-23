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

#include <stdlib.h>

#include "Platform.h"

#include "SDLWindow.h"
#include "Callbacks.h"

#include "UI/UIView.h"
#include "LGL/LGL.h"

#if defined( OS_WINDOWS )
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

extern std::unique_ptr<sLGLAPI> LGL3;

void SetHighDPIAware()
{
#if defined( OS_WINDOWS )
#pragma warning( disable:4996 ) // 'GetVersion' was declared deprecated
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;

	dwVersion = GetVersion();

	// Get the Windows version.
	dwMajorVersion = ( DWORD )( LOBYTE( LOWORD( dwVersion ) ) );

	if ( dwMajorVersion > 5 )
	{
		/// Anything below WinXP without SP3 does not support High DPI, so we do not enable it on WinXP at all.
		HMODULE Lib = ::LoadLibrary( "user32.dll" );

		typedef BOOL( *SetProcessDPIAwareFunc )();

		SetProcessDPIAwareFunc SetDPIAware = ( SetProcessDPIAwareFunc )::GetProcAddress( Lib, "SetProcessDPIAware" );

		if ( SetDPIAware ) { SetDPIAware(); }

		FreeLibrary( Lib );
	}

#endif
}

clPtr<clSDLWindow> clSDLWindow::CreateSDLWindow( const std::string& Title, int Width, int Height )
{
	return make_intrusive<clSDLWindow>( Title, Width, Height );
}

clSDLWindow::clSDLWindow( const std::string& Title, int Width, int Height )
	: m_Width( Width )
	, m_Height( Height )
	, m_PendingExit( false )
{
	SetHighDPIAware();

	m_Window = SDL_CreateWindow( Title.c_str(),
	                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                             Width, Height,
	                             SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	                           );

	SDL_MaximizeWindow( m_Window );

#ifdef ANDROID
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
#else
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_EGL, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

	m_Context = SDL_GL_CreateContext( m_Window );

	if ( !m_Context )
	{
		SDL_DestroyWindow( m_Window );
		exit( 255 );
	}

	SDL_GL_SetSwapInterval( 1 );
}

clSDLWindow::~clSDLWindow()
{
	SDL_DestroyWindow( m_Window );
}

void clSDLWindow::Swap()
{
	SDL_GL_SwapWindow( m_Window );
}

bool clSDLWindow::HandleEvent( const SDL_Event& Event )
{
	switch ( Event.type )
	{
		case SDL_WINDOWEVENT:
			if ( Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
			{
				m_Width  = Event.window.data1;
				m_Height = Event.window.data2;

				LGL3->glViewport( 0, 0, m_Width, m_Height );

				UpdateRootViewSize();
			}

			return true;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			OnKey( Event.key.keysym.sym, Event.type == SDL_KEYDOWN );
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			OnTouch( Event.button.x, Event.button.y, Event.type == SDL_MOUSEBUTTONDOWN );
			break;

		case SDL_MOUSEMOTION:
			OnMove( Event.motion.x, Event.motion.y );
			break;

		case SDL_MOUSEWHEEL:
			break;
	}

	return true;
}

void clSDLWindow::UpdateRootViewSize()
{
	if ( m_RootView )
	{
		m_RootView->SetSize( m_Width, m_Height );
		m_RootView->LayoutChildViews();
	}
}

bool clSDLWindow::HandleInput()
{
	SDL_Event Event;

	while ( SDL_PollEvent( &Event ) && !m_PendingExit )
	{
		if ( ( Event.type == SDL_QUIT ) || !this->HandleEvent( Event ) ) { m_PendingExit = true; }
	}

	return !m_PendingExit;
}

void clSDLWindow::RequestExit()
{
	m_PendingExit = true;
}

vec2 clSDLWindow::GetNormalizedPoint( int x, int y ) const
{
	return vec2(
	          static_cast<float>( x ) / m_Width,
	          static_cast<float>( y ) / m_Height
	       );
}
