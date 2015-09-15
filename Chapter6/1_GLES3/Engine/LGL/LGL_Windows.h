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

#include <stdio.h>

#include <SDL2/SDL.h>

// written for OpenGL 4.5

std::string LGL::GetExtensionsString( sLGLAPI* LGL3 )
{
	std::string Extensions;

	if ( LGL3->glGetStringi != nullptr )
	{
		// GL3 functionality
		GLint NumExt = 0;

		LGL3->glGetIntegerv( GL_NUM_EXTENSIONS, &NumExt );

		for ( GLint i = 0; i != NumExt; i++ )
		{
			Extensions += std::string( ( const char* )LGL3->glGetStringi( GL_EXTENSIONS, i ) ) + " ";
		}
	}
	else
	{
		if ( LGL3->glGetString != nullptr )
		{
			// old functionality
			const char* glStr = reinterpret_cast<const char*>( LGL3->glGetString( GL_EXTENSIONS ) );
			Extensions = glStr ? std::string( glStr ) : std::string();
		}
	}

	return Extensions;
}

void* LGL::GetGLProc( sLGLAPI* LGL3, const char* Name )
{
	return SDL_GL_GetProcAddress( Name );
}

void LGL::GetAPI( sLGLAPI* API )
{
	API->glActiveTexture = ( PFNGLACTIVETEXTUREPROC )GetGLProc( API, "glActiveTexture" );
	API->glAttachShader = ( PFNGLATTACHSHADERPROC )GetGLProc( API, "glAttachShader" );
	API->glBindAttribLocation = ( PFNGLBINDATTRIBLOCATIONPROC )GetGLProc( API, "glBindAttribLocation" );
	API->glBindBuffer = ( PFNGLBINDBUFFERPROC )GetGLProc( API, "glBindBuffer" );
	API->glBindBufferBase = ( PFNGLBINDBUFFERBASEPROC )GetGLProc( API, "glBindBufferBase" );
	API->glBindFragDataLocation = ( PFNGLBINDFRAGDATALOCATIONPROC )GetGLProc( API, "glBindFragDataLocation" );
	API->glBindFramebuffer = ( PFNGLBINDFRAMEBUFFERPROC )GetGLProc( API, "glBindFramebuffer" );
	API->glBindTexture = ( PFNGLBINDTEXTUREPROC )GetGLProc( API, "glBindTexture" );
	API->glBindVertexArray = ( PFNGLBINDVERTEXARRAYPROC )GetGLProc( API, "glBindVertexArray" );
	API->glBlendFunc = ( PFNGLBLENDFUNCPROC )GetGLProc( API, "glBlendFunc" );
	API->glBufferData = ( PFNGLBUFFERDATAPROC )GetGLProc( API, "glBufferData" );
	API->glBufferSubData = ( PFNGLBUFFERSUBDATAPROC )GetGLProc( API, "glBufferSubData" );
	API->glCheckFramebufferStatus = ( PFNGLCHECKFRAMEBUFFERSTATUSPROC )GetGLProc( API, "glCheckFramebufferStatus" );
	API->glClear = ( PFNGLCLEARPROC )GetGLProc( API, "glClear" );
	API->glClearColor = ( PFNGLCLEARCOLORPROC )GetGLProc( API, "glClearColor" );
	API->glClearDepth = ( PFNGLCLEARDEPTHPROC )GetGLProc( API, "glClearDepth" );
	API->glClearStencil = ( PFNGLCLEARSTENCILPROC )GetGLProc( API, "glClearStencil" );
	API->glColorMask = ( PFNGLCOLORMASKPROC )GetGLProc( API, "glColorMask" );
	API->glCompileShader = ( PFNGLCOMPILESHADERPROC )GetGLProc( API, "glCompileShader" );
	API->glCompressedTexImage2D = ( PFNGLCOMPRESSEDTEXIMAGE2DPROC )GetGLProc( API, "glCompressedTexImage2D" );
	API->glCompressedTexImage3D = ( PFNGLCOMPRESSEDTEXIMAGE3DPROC )GetGLProc( API, "glCompressedTexImage3D" );
	API->glCreateProgram = ( PFNGLCREATEPROGRAMPROC )GetGLProc( API, "glCreateProgram" );
	API->glCreateShader = ( PFNGLCREATESHADERPROC )GetGLProc( API, "glCreateShader" );
	API->glCullFace = ( PFNGLCULLFACEPROC )GetGLProc( API, "glCullFace" );
	API->glDeleteBuffers = ( PFNGLDELETEBUFFERSPROC )GetGLProc( API, "glDeleteBuffers" );
	API->glDeleteFramebuffers = ( PFNGLDELETEFRAMEBUFFERSPROC )GetGLProc( API, "glDeleteFramebuffers" );
	API->glDeleteProgram = ( PFNGLDELETEPROGRAMPROC )GetGLProc( API, "glDeleteProgram" );
	API->glDeleteShader = ( PFNGLDELETESHADERPROC )GetGLProc( API, "glDeleteShader" );
	API->glDeleteTextures = ( PFNGLDELETETEXTURESPROC )GetGLProc( API, "glDeleteTextures" );
	API->glDeleteVertexArrays = ( PFNGLDELETEVERTEXARRAYSPROC )GetGLProc( API, "glDeleteVertexArrays" );
	API->glDepthFunc = ( PFNGLDEPTHFUNCPROC )GetGLProc( API, "glDepthFunc" );
	API->glDepthMask = ( PFNGLDEPTHMASKPROC )GetGLProc( API, "glDepthMask" );
	API->glDisable = ( PFNGLDISABLEPROC )GetGLProc( API, "glDisable" );
	API->glDisableVertexAttribArray = ( PFNGLDISABLEVERTEXATTRIBARRAYPROC )GetGLProc( API, "glDisableVertexAttribArray" );
	API->glDisablei = ( PFNGLDISABLEIPROC )GetGLProc( API, "glDisablei" );
	API->glDrawArrays = ( PFNGLDRAWARRAYSPROC )GetGLProc( API, "glDrawArrays" );
	API->glDrawArraysInstanced = ( PFNGLDRAWARRAYSINSTANCEDPROC )GetGLProc( API, "glDrawArraysInstanced" );
	API->glDrawBuffers = ( PFNGLDRAWBUFFERSPROC )GetGLProc( API, "glDrawBuffers" );
	API->glDrawElements = ( PFNGLDRAWELEMENTSPROC )GetGLProc( API, "glDrawElements" );
	API->glDrawElementsInstanced = ( PFNGLDRAWELEMENTSINSTANCEDPROC )GetGLProc( API, "glDrawElementsInstanced" );
	API->glEnable = ( PFNGLENABLEPROC )GetGLProc( API, "glEnable" );
	API->glEnableVertexAttribArray = ( PFNGLENABLEVERTEXATTRIBARRAYPROC )GetGLProc( API, "glEnableVertexAttribArray" );
	API->glEnablei = ( PFNGLENABLEIPROC )GetGLProc( API, "glEnablei" );
	API->glFinish = ( PFNGLFINISHPROC )GetGLProc( API, "glFinish" );
	API->glFlush = ( PFNGLFLUSHPROC )GetGLProc( API, "glFlush" );
	API->glFramebufferTexture2D = ( PFNGLFRAMEBUFFERTEXTURE2DPROC )GetGLProc( API, "glFramebufferTexture2D" );
	API->glGenBuffers = ( PFNGLGENBUFFERSPROC )GetGLProc( API, "glGenBuffers" );
	API->glGenFramebuffers = ( PFNGLGENFRAMEBUFFERSPROC )GetGLProc( API, "glGenFramebuffers" );
	API->glGenTextures = ( PFNGLGENTEXTURESPROC )GetGLProc( API, "glGenTextures" );
	API->glGenVertexArrays = ( PFNGLGENVERTEXARRAYSPROC )GetGLProc( API, "glGenVertexArrays" );
	API->glGenerateMipmap = ( PFNGLGENERATEMIPMAPPROC )GetGLProc( API, "glGenerateMipmap" );
	API->glGetActiveAttrib = ( PFNGLGETACTIVEATTRIBPROC )GetGLProc( API, "glGetActiveAttrib" );
	API->glGetActiveUniform = ( PFNGLGETACTIVEUNIFORMPROC )GetGLProc( API, "glGetActiveUniform" );
	API->glGetActiveUniformBlockiv = ( PFNGLGETACTIVEUNIFORMBLOCKIVPROC )GetGLProc( API, "glGetActiveUniformBlockiv" );
	API->glGetAttribLocation = ( PFNGLGETATTRIBLOCATIONPROC )GetGLProc( API, "glGetAttribLocation" );
	API->glGetError = ( PFNGLGETERRORPROC )GetGLProc( API, "glGetError" );
	API->glGetIntegerv = ( PFNGLGETINTEGERVPROC )GetGLProc( API, "glGetIntegerv" );
	API->glGetProgramInfoLog = ( PFNGLGETPROGRAMINFOLOGPROC )GetGLProc( API, "glGetProgramInfoLog" );
	API->glGetProgramStageiv = ( PFNGLGETPROGRAMSTAGEIVPROC )GetGLProc( API, "glGetProgramStageiv" );
	API->glGetProgramiv = ( PFNGLGETPROGRAMIVPROC )GetGLProc( API, "glGetProgramiv" );
	API->glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC )GetGLProc( API, "glGetShaderInfoLog" );
	API->glGetShaderiv = ( PFNGLGETSHADERIVPROC )GetGLProc( API, "glGetShaderiv" );
	API->glGetString = ( PFNGLGETSTRINGPROC )GetGLProc( API, "glGetString" );
	API->glGetStringi = ( PFNGLGETSTRINGIPROC )GetGLProc( API, "glGetStringi" );
	API->glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC )GetGLProc( API, "glGetUniformLocation" );
	API->glIsProgram = ( PFNGLISPROGRAMPROC )GetGLProc( API, "glIsProgram" );
	API->glIsShader = ( PFNGLISSHADERPROC )GetGLProc( API, "glIsShader" );
	API->glLinkProgram = ( PFNGLLINKPROGRAMPROC )GetGLProc( API, "glLinkProgram" );
	API->glPixelStorei = ( PFNGLPIXELSTOREIPROC )GetGLProc( API, "glPixelStorei" );
	API->glReadBuffer = ( PFNGLREADBUFFERPROC )GetGLProc( API, "glReadBuffer" );
	API->glReadPixels = ( PFNGLREADPIXELSPROC )GetGLProc( API, "glReadPixels" );
	API->glScissor = ( PFNGLSCISSORPROC )GetGLProc( API, "glScissor" );
	API->glShaderSource = ( PFNGLSHADERSOURCEPROC )GetGLProc( API, "glShaderSource" );
	API->glTexImage2D = ( PFNGLTEXIMAGE2DPROC )GetGLProc( API, "glTexImage2D" );
	API->glTexImage3D = ( PFNGLTEXIMAGE3DPROC )GetGLProc( API, "glTexImage3D" );
	API->glTexParameterf = ( PFNGLTEXPARAMETERFPROC )GetGLProc( API, "glTexParameterf" );
	API->glTexParameterfv = ( PFNGLTEXPARAMETERFVPROC )GetGLProc( API, "glTexParameterfv" );
	API->glTexParameteri = ( PFNGLTEXPARAMETERIPROC )GetGLProc( API, "glTexParameteri" );
	API->glTexParameteriv = ( PFNGLTEXPARAMETERIVPROC )GetGLProc( API, "glTexParameteriv" );
	API->glTexSubImage2D = ( PFNGLTEXSUBIMAGE2DPROC )GetGLProc( API, "glTexSubImage2D" );
	API->glUniform1f = ( PFNGLUNIFORM1FPROC )GetGLProc( API, "glUniform1f" );
	API->glUniform1fv = ( PFNGLUNIFORM1FVPROC )GetGLProc( API, "glUniform1fv" );
	API->glUniform1i = ( PFNGLUNIFORM1IPROC )GetGLProc( API, "glUniform1i" );
	API->glUniform1iv = ( PFNGLUNIFORM1IVPROC )GetGLProc( API, "glUniform1iv" );
	API->glUniform3fv = ( PFNGLUNIFORM3FVPROC )GetGLProc( API, "glUniform3fv" );
	API->glUniform4fv = ( PFNGLUNIFORM4FVPROC )GetGLProc( API, "glUniform4fv" );
	API->glUniformBlockBinding = ( PFNGLUNIFORMBLOCKBINDINGPROC )GetGLProc( API, "glUniformBlockBinding" );
	API->glUniformMatrix3fv = ( PFNGLUNIFORMMATRIX3FVPROC )GetGLProc( API, "glUniformMatrix3fv" );
	API->glUniformMatrix4fv = ( PFNGLUNIFORMMATRIX4FVPROC )GetGLProc( API, "glUniformMatrix4fv" );
	API->glUseProgram = ( PFNGLUSEPROGRAMPROC )GetGLProc( API, "glUseProgram" );
	API->glValidateProgram = ( PFNGLVALIDATEPROGRAMPROC )GetGLProc( API, "glValidateProgram" );
	API->glVertexAttribPointer = ( PFNGLVERTEXATTRIBPOINTERPROC )GetGLProc( API, "glVertexAttribPointer" );
	API->glViewport = ( PFNGLVIEWPORTPROC )GetGLProc( API, "glViewport" );
}
