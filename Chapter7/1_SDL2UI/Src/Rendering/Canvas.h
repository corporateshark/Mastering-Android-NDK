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

#include "Platform.h"
#include "IntrusivePtr.h"
#include "SDLWindow.h"
#include "VecMath.h"

class clBitmap;
class clVertexAttribs;
class clGLVertexArray;
class clGLSLShaderProgram;
class clGLTexture;

class iCanvas: public iIntrusiveCounter
{
public:

	virtual void SetColor( int R, int G, int B ) = 0;
	virtual void SetColor( const ivec4& C ) = 0;
	virtual void Clear() = 0;
	virtual void Rect( int X, int Y, int W, int H, bool Filled ) = 0;

	virtual int CreateTexture( const clPtr<clBitmap>& Pixels ) = 0;
	virtual void UpdateTexture( int Idx, const clPtr<clBitmap>& Pixels ) = 0;
	virtual void DeleteTexture( int Idx ) = 0;

	virtual void TextureRect( int X, int Y, int W, int H, int SX, int SY, int SW, int SH, int idx ) = 0;
	virtual void TextureRect( int X, int Y, int W, int H, const clPtr<clGLTexture>& Texture ) = 0;

	virtual void TextStr( int X1, int Y1, int X2, int Y2, const std::string& Str, int Size, const ivec4& Color, int FontID ) = 0;

	/// make everything visible
	virtual void Present() = 0;
};

class clSDLCanvas: public iCanvas
{
public:
	explicit clSDLCanvas( const clPtr<clSDLWindow>& Window );
	virtual ~clSDLCanvas();

	//
	// iCanvas interface
	//
	virtual void SetColor( int R, int G, int B ) override;
	virtual void SetColor( const ivec4& C ) override;
	virtual void Clear() override;
	virtual void Rect( int X, int Y, int W, int H, bool Filled ) override;
	virtual void Present() override;

	virtual int CreateTexture( const clPtr<clBitmap>& Pixels ) override;
	virtual void UpdateTexture( int Idx, const clPtr<clBitmap>& Pixels ) override;
	virtual void DeleteTexture( int Idx ) override;

	virtual void TextureRect( int X, int Y, int W, int H, int SX, int SY, int SW, int SH, int idx ) override;
	virtual void TextureRect( int X, int Y, int W, int H, const clPtr<clGLTexture>& Texture ) override {}

	virtual void TextStr( int X1, int Y1, int X2, int Y2, const std::string& Str, int Size, const ivec4& Color, int FontID ) override;

private:
	SDL_Renderer* m_Renderer;

	std::vector<SDL_Texture*> m_Textures;

	int m_TextTexture;
};

class clGLCanvas: public iCanvas
{
public:
	explicit clGLCanvas( const clPtr<clSDLWindow>& Window );
	virtual ~clGLCanvas();

	//
	// iCanvas interface
	//
	virtual void SetColor( int R, int G, int B ) override;
	virtual void SetColor( const ivec4& C ) override;
	virtual void Clear() override;
	virtual void Rect( int X, int Y, int W, int H, bool Filled ) override;
	virtual void Present() override;

	virtual int CreateTexture( const clPtr<clBitmap>& Pixels ) override;
	virtual void UpdateTexture( int Idx, const clPtr<clBitmap>& Pixels ) override;
	virtual void DeleteTexture( int Idx ) override;

	virtual void TextureRect( int X, int Y, int W, int H, int SX, int SY, int SW, int SH, int idx ) override;
	virtual void TextureRect( int X, int Y, int W, int H, const clPtr<clGLTexture>& Texture ) override;

	virtual void TextStr( int X1, int Y1, int X2, int Y2, const std::string& Str, int Size, const ivec4& Color, int FontID ) override;

private:
	vec4 ConvertScreenToNDC( int X, int Y, int W, int H ) const;

private:
	clPtr<clSDLWindow> m_Window;
	vec4 m_Color;
	clPtr<clVertexAttribs> m_Rect;
	clPtr<clGLVertexArray> m_RectVA;
	clPtr<clGLSLShaderProgram> m_RectSP;
	clPtr<clGLSLShaderProgram> m_TexRectSP;
	std::vector<clPtr<clGLTexture>> m_Textures;
};
