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
#include <SDL2/SDL.h>

#include "Engine/LGL/LGL.h"
#include "Engine/LGL/GLClasses.h"
#include "Engine/Graphics/Geometry.h"
#include "Engine/SDLLibrary.h"
#include "Engine/SDLWindow.h"
#include "Engine/Callbacks.h"
#include "Engine/Log.h"

#if !defined( ANDROID )
#  undef main
#endif

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clSDLWindow> g_Window;

// geometry
clPtr<clVertexAttribs> g_Box;
clPtr<clGLVertexArray> g_BoxVA;
clPtr<clGLSLShaderProgram> g_ShaderProgram;

static const char g_vShaderStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
   in vec4 in_Vertex;
   in vec2 in_TexCoord;
   out vec2 Coords;
   void main()
   {
      Coords = in_TexCoord.xy;
      gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
   }
	)";

static const char g_fShaderStr[] =
   R"(
   in vec2 Coords;
   out vec4 out_FragColor;
   void main()
   {
      out_FragColor = vec4( Coords, 1.0, 1.0 );
   }
	)";

void Fatal( const char* Msg )
{
	LOGI( "%s: %s\n", Msg, SDL_GetError() );

	SDL_Quit();

	exit( 255 );
}

void OnStart( const std::string& RootPath )
{
	LOGI( "Hello Android NDK!" );

	const char* GLVersion  = ( const char* )LGL3->glGetString( GL_VERSION  );
	const char* GLVendor   = ( const char* )LGL3->glGetString( GL_VENDOR   );
	const char* GLRenderer = ( const char* )LGL3->glGetString( GL_RENDERER );

	LOGI( "GLVersion : %s\n", GLVersion );
	LOGI( "GLVendor  : %s\n", GLVendor  );
	LOGI( "GLRenderer: %s\n", GLRenderer );

	g_Box = clGeomServ::CreateAxisAlignedBox( LVector3( -1 ), LVector3( +1 ) );

	g_BoxVA = make_intrusive<clGLVertexArray>();
	g_BoxVA->SetVertexAttribs( g_Box );

	g_ShaderProgram = make_intrusive<clGLSLShaderProgram>( g_vShaderStr, g_fShaderStr );

	LGL3->glClearColor( 0.1f, 0.0f, 0.0f, 1.0f );
	LGL3->glEnable( GL_DEPTH_TEST );
}

void OnTouch( int X, int Y, bool Touch )
{
}

void OnMove( int X, int Y )
{
}

void OnDrawFrame()
{
	static float Angle = 0;

	Angle += 0.02f;

	LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	LMatrix4 Proj = Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f );
	LMatrix4 MV = LMatrix4::GetRotateMatrixAxis( Angle, LVector3( 1, 1, 1 ) ) * LMatrix4::GetTranslateMatrix( LVector3( 0, 0, -5 ) );

	g_ShaderProgram->Bind();
	g_ShaderProgram->SetUniformNameMat4Array( "in_ModelViewProjectionMatrix", 1, MV * Proj );

	g_BoxVA->Draw( false );
}

void OnKey( int KeyCode, bool Pressed )
{
	g_Window->RequestExit();
}

int main( int argc, char* argv[] )
{
	clSDL SDLLibrary;

	g_Window = clSDLWindow::CreateSDLWindow( "GLES3", 1024, 768 );

	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

	OnStart( "" );

	while ( g_Window && g_Window->HandleInput() )
	{
		OnDrawFrame();

		g_Window->Swap();
	}

	g_Window = nullptr;

	return 0;
}
