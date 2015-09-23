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

#include "Platform.h"
#include "GLFrameBuffer.h"
#include "Log.h"

int g_ScreenWidth = 0;
int g_ScreenHeight = 0;

extern std::unique_ptr<sLGLAPI> LGL3;

void clGLFrameBuffer::InitRenderTargetV( const ivec4& WidthHeightBitsPerChannel, const bool HasDepthBuffer )
{
	FColorBuffersParams = WidthHeightBitsPerChannel;
	FHasDepthBuffer     = HasDepthBuffer;

	LGL3->glGenFramebuffers( 1, &FFrameBuffer );

	Bind( 0 );

	// new color buffer
	FColorBuffer = make_intrusive<clGLTexture>();

	int Width       = FColorBuffersParams[0];
	int Height      = FColorBuffersParams[1];
	int BitsPerChan = FColorBuffersParams[2];

#if defined(OS_ANDROID) || defined(OS_EMSCRIPTEN)
	const Lenum InternalFormat = GL_RGBA;
	auto DepthFormat = GL_DEPTH_COMPONENT;
#else
	const Lenum InternalFormat = GL_RGBA8;
	auto DepthFormat = GL_DEPTH_COMPONENT24;
#endif

	FColorBuffer->SetFormat( GL_TEXTURE_2D, InternalFormat, GL_RGBA, Width, Height );
	FColorBuffer->AttachToCurrentFB( GL_COLOR_ATTACHMENT0 );

	// depth buffer
	if ( HasDepthBuffer )
	{
		FDepthBuffer = make_intrusive<clGLTexture>();

		int Width  = FColorBuffersParams[0];
		int Height = FColorBuffersParams[1];

		FDepthBuffer->SetFormat( GL_TEXTURE_2D, DepthFormat, GL_DEPTH_COMPONENT, Width, Height );
		FDepthBuffer->AttachToCurrentFB( GL_DEPTH_ATTACHMENT );
	}

	CheckFrameBuffer();

	UnBind();
}

LVector4i clGLFrameBuffer::GetParameters() const
{
	return FColorBuffersParams;
}

clGLFrameBuffer::~clGLFrameBuffer()
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	LGL3->glDeleteFramebuffers( 1, &FFrameBuffer );
}

void clGLFrameBuffer::CheckFrameBuffer() const
{
	Bind( 0 );

	GLenum FBStatus = LGL3->glCheckFramebufferStatus( GL_FRAMEBUFFER );

	switch ( FBStatus )
	{
		case GL_FRAMEBUFFER_COMPLETE: // Everything's OK
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			LOGI( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			LOGI( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
			break;

#if !defined(OS_ANDROID) && !defined(OS_EMSCRIPTEN)

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			LOGI( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			LOGI( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
			break;
#endif

		case GL_FRAMEBUFFER_UNSUPPORTED:
			LOGI( "GL_FRAMEBUFFER_UNSUPPORTED" );
			break;

		default:
			LOGI( "Unknown framebuffer error: %x", FBStatus );
	}

	UnBind();
}

void clGLFrameBuffer::Bind( int TargetIndex ) const
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, FFrameBuffer );

	int Width  = FColorBuffersParams[0];
	int Height = FColorBuffersParams[1];

	LGL3->glViewport( 0, 0, Width, Height );
}

void clGLFrameBuffer::UnBind() const
{
	LGL3->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	LGL3->glViewport( 0, 0, g_ScreenWidth, g_ScreenHeight );
}

clPtr<clGLTexture> clGLFrameBuffer::GetColorTexture() const
{
	return FColorBuffer;
}

clPtr<clGLTexture> clGLFrameBuffer::GetDepthTexture() const
{
	return FDepthBuffer;
}
