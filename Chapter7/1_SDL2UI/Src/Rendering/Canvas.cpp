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

#include <SDL2/SDL.h>
#include "Canvas.h"
#include "Bitmap.h"
#include "TextRenderer.h"
#include "SDLWindow.h"
#include "Log.h"
#include "LGL/LGL.h"
#include "LGL/GLClasses.h"
#include "Geometry.h"

clSDLCanvas::clSDLCanvas( const clPtr<clSDLWindow>& Window )
	: m_Renderer( nullptr )
{
	m_Renderer = SDL_CreateRenderer( Window->GetSDLWindow(), -1, SDL_RENDERER_ACCELERATED );

	if ( !m_Renderer )
	{
		LOGI( "Renderer was not created: %s", SDL_GetError() );
		return;
	}
}

clSDLCanvas::~clSDLCanvas()
{
	SDL_DestroyRenderer( m_Renderer );
}

void clSDLCanvas::SetColor( int R, int G, int B )
{
	SDL_SetRenderDrawColor( m_Renderer, R, G, B, 0xFF );
}

void clSDLCanvas::SetColor( const ivec4& C )
{
	SDL_SetRenderDrawColor( m_Renderer, C.x, C.y, C.z, C.w );
}

void clSDLCanvas::Clear()
{
	SDL_RenderClear( m_Renderer );
}

void clSDLCanvas::Rect( int X, int Y, int W, int H, bool Filled )
{
	SDL_Rect R = { X, Y, W, H };

	if ( Filled )
	{
		SDL_RenderFillRect( m_Renderer, &R );
	}
	else
	{
		SDL_RenderDrawRect( m_Renderer, &R );
	}
}

void clSDLCanvas::Present()
{
	SDL_RenderPresent( m_Renderer );
}

int clSDLCanvas::CreateTexture( const clPtr<clBitmap>& Pixels )
{
	if ( !Pixels ) { return -1; }

	SDL_Texture* Tex = SDL_CreateTexture(
	                      m_Renderer,
	                      SDL_PIXELFORMAT_RGBA8888,
	                      SDL_TEXTUREACCESS_STREAMING, Pixels->GetWidth(), Pixels->GetHeight()
	                   );

	SDL_Rect Rect = { 0, 0, Pixels->GetWidth(), Pixels->GetHeight() };

	void* TexturePixels = nullptr;
	int Pitch = 0;
	int Result = SDL_LockTexture( Tex, &Rect, &TexturePixels, &Pitch );

	memcpy( TexturePixels, Pixels->FBitmapData, Pitch * Pixels->GetHeight() );

	SDL_UnlockTexture( Tex );

	int Idx = ( int )m_Textures.size();

	m_Textures.push_back( Tex );

	return Idx;
}

void clSDLCanvas::UpdateTexture( int Idx, const clPtr<clBitmap>& Pixels )
{
	if ( !Pixels ) { return; }

	if ( !Pixels || Idx < 0 || Idx >= ( int )m_Textures.size() )
	{
		return;
	}

	Uint32 Fmt;
	int Access;
	int W, H;
	SDL_QueryTexture( m_Textures[Idx], &Fmt, &Access, &W, &H );

	SDL_Rect Rect = { 0, 0, W, H };

	void* TexturePixels = nullptr;
	int Pitch = 0;
	int res = SDL_LockTexture( m_Textures[Idx], &Rect, &TexturePixels, &Pitch );

	memcpy( TexturePixels, Pixels->FBitmapData, Pitch * H );

	SDL_UnlockTexture( m_Textures[Idx] );
}

void clSDLCanvas::DeleteTexture( int Idx )
{
	if ( Idx < 0 || Idx >= ( int )m_Textures.size() )
	{
		return;
	}

	SDL_DestroyTexture( m_Textures[Idx] );

	m_Textures[Idx] = 0;
}

void clSDLCanvas::TextureRect( int X, int Y, int W, int H, int SX, int SY, int SW, int SH, int Idx )
{
	SDL_Rect DstRect = { X, Y, X + W, Y + H };
	SDL_Rect SrcRect = { SX, SY, SX + SW, SY + SH };

	SDL_SetTextureBlendMode( m_Textures[Idx], SDL_BLENDMODE_BLEND );
	int Result = SDL_RenderCopy( m_Renderer, m_Textures[Idx], &SrcRect, &DstRect );
}

void clSDLCanvas::TextStr( int X1, int Y1, int X2, int Y2, const std::string& Str, int Size, const ivec4& Color, int FontID )
{
	clPtr<clBitmap> B = clTextRenderer::Instance()->RenderTextWithFont( Str, FontID, Size, Color, true );

	UpdateTexture( m_TextTexture, B );

	// TODO: use X2-X1 as a real output size, not as an input
	int SW = X2 - X1 + 1, SH = Y2 - Y1 + 1;

	this->TextureRect( X1, Y1, X2 - X1 + 1, Y2 - Y1 + 1, 0, 0, SW, SH, m_TextTexture );
}

extern std::unique_ptr<sLGLAPI> LGL3;

static const char RectvShaderStr[] =
   R"(
uniform vec4 u_RectSize;
in vec4 in_Vertex;
in vec2 in_TexCoord;
out vec2 Coords;
void main()
{
   Coords = in_TexCoord;
   float X1 = u_RectSize.x;
   float Y1 = u_RectSize.y;
   float X2 = u_RectSize.z;
   float Y2 = u_RectSize.w;
   float Width  = X2 - X1;
   float Height = Y2 - Y1;
   vec4 VertexPos = vec4( X1 + in_Vertex.x * Width, Y1 + in_Vertex.y * Height, in_Vertex.z, in_Vertex.w ) * vec4( 2.0, -2.0, 1.0, 1.0 ) + vec4( -1.0, 1.0, 0.0, 0.0 );
   gl_Position = VertexPos;
}
)";

static const char RectfShaderStr[] =
R"(
uniform vec4 u_Color;
out vec4 out_FragColor;
in vec2 Coords;
void main()
{
   out_FragColor = u_Color;
}
)";

static const char TexRectfShaderStr[] =
R"(
uniform vec4 u_Color;
out vec4 out_FragColor;
in vec2 Coords;
uniform sampler2D Texture0;
void main()
{
   out_FragColor = u_Color * texture( Texture0, Coords );
}
)";

clGLCanvas::clGLCanvas( const clPtr<clSDLWindow>& Window )
: m_Window( Window )
{
	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

	m_Rect = clGeomServ::CreateRect2D( 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, 1 );

	m_RectVA = new clGLVertexArray();
	m_RectVA->SetVertexAttribs( m_Rect );

	m_RectSP = new clGLSLShaderProgram( RectvShaderStr, RectfShaderStr );
	m_TexRectSP = new clGLSLShaderProgram( RectvShaderStr, TexRectfShaderStr );
}

clGLCanvas::~clGLCanvas()
{
	LGL3 = nullptr;
}

void clGLCanvas::SetColor( int R, int G, int B )
{
	m_Color = vec4( R, G, B, 255 ) / 255.0f;
}

void clGLCanvas::SetColor( const ivec4& C )
{
	m_Color = vec4(C) / 255.0f;
}

void clGLCanvas::Clear()
{
	LGL3->glClearColor( m_Color.x, m_Color.y, m_Color.z, m_Color.w );
	LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

vec4 clGLCanvas::ConvertScreenToNDC( int X, int Y, int W, int H ) const
{
	float WinW = static_cast<float>( m_Window->GetWidth() );
	float WinH = static_cast<float>( m_Window->GetHeight() );

	vec4 Pos(
		static_cast<float>( X ) / WinW, static_cast<float>( Y ) / WinH,
		static_cast<float>( X + W ) / WinW, static_cast<float>( Y + H ) / WinH );

	return Pos;
}

void clGLCanvas::Rect( int X, int Y, int W, int H, bool Filled )
{
	vec4 Pos = ConvertScreenToNDC( X, Y, W, H );

	LGL3->glDisable( GL_DEPTH_TEST );

	m_RectSP->Bind();
	m_RectSP->SetUniformNameVec4Array( "u_Color", 1, m_Color );
	m_RectSP->SetUniformNameVec4Array( "u_RectSize", 1, Pos );

	if ( m_Color.w < 1.0f )
	{
		LGL3->glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		LGL3->glEnable( GL_BLEND );
	}

	m_RectVA->Draw( false );

	if ( m_Color.w < 1.0f )
	{
		LGL3->glDisable( GL_BLEND );
	}
}

void clGLCanvas::Present()
{
	m_Window->Swap();
}

int clGLCanvas::CreateTexture( const clPtr<clBitmap>& Pixels )
{
	if ( !Pixels ) return -1;

	m_Textures.emplace_back( new clGLTexture() );
	m_Textures.back()->LoadFromBitmap( Pixels );

	return m_Textures.size()-1;
}

void clGLCanvas::UpdateTexture( int Idx, const clPtr<clBitmap>& Pixels )
{
	if ( !Pixels || Idx < 0 || Idx >= (int)m_Textures.size() )
	{
		return;
	}

	if ( m_Textures[ Idx ] ) m_Textures[ Idx ]->LoadFromBitmap( Pixels );
}

void clGLCanvas::DeleteTexture( int Idx )
{
	if ( Idx < 0 || Idx >= (int)m_Textures.size() )
	{
		return;
	}

	m_Textures[ Idx ] = nullptr;
}

void clGLCanvas::TextureRect( int X, int Y, int W, int H, int SX, int SY, int SW, int SH, int Idx )
{
	if ( Idx < 0 || Idx >= (int)m_Textures.size() )
	{
		return;
	}

	vec4 Pos = ConvertScreenToNDC( X, Y, W, H );

	LGL3->glDisable( GL_DEPTH_TEST );

	m_Textures[ Idx ]->Bind( 0 );

	m_TexRectSP->Bind();
	m_TexRectSP->SetUniformNameVec4Array( "u_Color", 1, m_Color );
	m_TexRectSP->SetUniformNameVec4Array( "u_RectSize", 1, Pos );

	LGL3->glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	LGL3->glEnable( GL_BLEND );

	m_RectVA->Draw( false );

	LGL3->glDisable( GL_BLEND );
}

void clGLCanvas::TextStr( int X1, int Y1, int X2, int Y2, const std::string& Str, int Size, const ivec4& Color, int FontID )
{
	clPtr<clBitmap> B = clTextRenderer::Instance()->RenderTextWithFont( Str, FontID, Size, Color, true );

	static int Texture = this->CreateTexture(B);

	UpdateTexture( Texture, B );

	int SW = X2 - X1 + 1;
	int SH = Y2 - Y1 + 1;

	this->TextureRect( X1, Y1, X2 - X1 + 1, Y2 - Y1 + 1, 0, 0, SW, SH, Texture );
}

void clGLCanvas::TextureRect( int X, int Y, int W, int H, const clPtr<clGLTexture>& Texture )
{
	vec4 Pos = ConvertScreenToNDC( X, Y, W, H );

	LGL3->glDisable( GL_DEPTH_TEST );

	Texture->Bind( 0 );

	m_TexRectSP->Bind();
	m_TexRectSP->SetUniformNameVec4Array( "u_Color", 1, m_Color );
	m_TexRectSP->SetUniformNameVec4Array( "u_RectSize", 1, Pos );

	LGL3->glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	LGL3->glEnable( GL_BLEND );

	m_RectVA->Draw( false );

	LGL3->glDisable( GL_BLEND );
}
