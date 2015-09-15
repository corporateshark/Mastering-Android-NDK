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

#include <SDL2/SDL.h>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "LGL3", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "LGL3", __VA_ARGS__))

// written for OpenGL ES 2
//		+ some optional features from OpenGL ES 3 (VAO)

std::string LGL::GetExtensionsString( sLGLAPI* LGL3 )
{
	if ( LGL3->glGetString != nullptr )
	{
		const char* glStr = reinterpret_cast<const char*>( LGL3->glGetString( GL_EXTENSIONS ) );

		if ( glStr ) { return std::string( glStr ); }
	}

	return std::string();
}

void* LGL::GetGLProc( sLGLAPI* LGL3, const char* Name )
{
	return SDL_GL_GetProcAddress( Name );
}

LGL_CALL void Emulate_glClearDepth( double Depth )
{
	glClearDepthf( static_cast<float>( Depth ) );
}

LGL_CALL void Emulate_glDisablei( GLenum mode, GLuint )
{
	glDisable( mode );
}

LGL_CALL void Emulate_glEnablei( GLenum mode, GLuint )
{
	glEnable( mode );
}

LGL_CALL void Emulate_glBindFragDataLocation( GLuint, GLuint, const GLchar* )
{
	// not supported
}

void LGL::GetAPI( sLGLAPI* API )
{
	LOGI( "Getting OpenGL ES 3 API for Android" );

	API->glActiveTexture = &glActiveTexture;
	API->glAttachShader = &glAttachShader;
	API->glBindAttribLocation = &glBindAttribLocation;
	API->glBindBuffer = &glBindBuffer;
	API->glBindBufferBase = &glBindBufferBase;
	API->glBindFragDataLocation = &Emulate_glBindFragDataLocation;
	API->glBindFramebuffer = &glBindFramebuffer;
	API->glBindTexture = &glBindTexture;
	API->glBindVertexArray = &glBindVertexArray;
	API->glBlendFunc = &glBlendFunc;
	API->glBufferData = &glBufferData;
	API->glBufferSubData = &glBufferSubData;
	API->glCheckFramebufferStatus = &glCheckFramebufferStatus;
	API->glClear = &glClear;
	API->glClearColor = &glClearColor;
	API->glClearDepth = &Emulate_glClearDepth;
	API->glClearStencil = &glClearStencil;
	API->glColorMask = &glColorMask;
	API->glCompileShader = &glCompileShader;
	API->glCompressedTexImage2D = &glCompressedTexImage2D;
	API->glCompressedTexImage3D = &glCompressedTexImage3D;
	API->glCreateProgram = &glCreateProgram;
	API->glCreateShader = &glCreateShader;
	API->glCullFace = &glCullFace;
	API->glDeleteBuffers = &glDeleteBuffers;
	API->glDeleteFramebuffers = &glDeleteFramebuffers;
	API->glDeleteProgram = &glDeleteProgram;
	API->glDeleteShader = &glDeleteShader;
	API->glDeleteTextures = &glDeleteTextures;
	API->glDeleteVertexArrays = &glDeleteVertexArrays;
	API->glDepthFunc = &glDepthFunc;
	API->glDepthMask = &glDepthMask;
	API->glDisable = &glDisable;
	API->glDisablei = &Emulate_glDisablei;
	API->glDisableVertexAttribArray = &glDisableVertexAttribArray;
	API->glDrawArrays = &glDrawArrays;
	API->glDrawArraysInstanced = &glDrawArraysInstanced;
	API->glDrawBuffers = &glDrawBuffers;
	API->glDrawElements = &glDrawElements;
	API->glDrawElementsInstanced = &glDrawElementsInstanced;
	API->glEnable = &glEnable;
	API->glEnablei = &Emulate_glEnablei;
	API->glEnableVertexAttribArray = &glEnableVertexAttribArray;
	API->glFinish = &glFinish;
	API->glFlush = &glFlush;
	API->glFramebufferTexture2D = &glFramebufferTexture2D;
	API->glGenBuffers = &glGenBuffers;
	API->glGenerateMipmap = &glGenerateMipmap;
	API->glGenFramebuffers = &glGenFramebuffers;
	API->glGenTextures = &glGenTextures;
	API->glGenVertexArrays = &glGenVertexArrays;
	API->glGetActiveAttrib = &glGetActiveAttrib;
	API->glGetActiveUniform = &glGetActiveUniform;
	API->glGetActiveUniformBlockiv = &glGetActiveUniformBlockiv;
	API->glGetAttribLocation = &glGetAttribLocation;
	API->glGetError = &glGetError;
	API->glGetIntegerv = &glGetIntegerv;
	API->glGetProgramInfoLog = &glGetProgramInfoLog;
	API->glGetProgramiv = &glGetProgramiv;
	API->glGetShaderInfoLog = &glGetShaderInfoLog;
	API->glGetShaderiv = &glGetShaderiv;
	API->glGetString = ( PFNGLGETSTRINGPROC )&glGetString;
	API->glGetUniformLocation = &glGetUniformLocation;
	API->glIsProgram = &glIsProgram;
	API->glIsShader = &glIsShader;
	API->glLinkProgram = &glLinkProgram;
	API->glPixelStorei = &glPixelStorei;
	API->glScissor = &glScissor;
	API->glProgramParameteri = &glProgramParameteri;
	API->glReadBuffer = &glReadBuffer;
	API->glReadPixels = &glReadPixels;
	// OpenGL ES 2 and OpenGL ES 3 have different "const" in prototype of glShaderSource
	API->glShaderSource = ( PFNGLSHADERSOURCEPROC )&glShaderSource;
	API->glTexSubImage2D = &glTexSubImage2D;
	API->glTexImage2D = &glTexImage2D;
	API->glTexImage3D = &glTexImage3D;
	API->glTexParameterf = &glTexParameterf;
	API->glTexParameteri = &glTexParameteri;
	API->glTexParameteriv = &glTexParameteriv;
	API->glTexParameterfv = &glTexParameterfv;
	API->glUniform1f = &glUniform1f;
	API->glUniform1fv = &glUniform1fv;
	API->glUniform1i = &glUniform1i;
	API->glUniform1iv = &glUniform1iv;
	API->glUniform3fv = &glUniform3fv;
	API->glUniform4fv = &glUniform4fv;
	API->glUniformBlockBinding = &glUniformBlockBinding;
	API->glUniformMatrix3fv = &glUniformMatrix3fv;
	API->glUniformMatrix4fv = &glUniformMatrix4fv;
	API->glUseProgram = &glUseProgram;
	API->glValidateProgram = &glValidateProgram;
	API->glVertexAttribPointer = &glVertexAttribPointer;
	API->glViewport = &glViewport;
}
