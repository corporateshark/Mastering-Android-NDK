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
#include "Library.h"

#ifdef OS_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

clLibrary::clLibrary()
	: FLibHandle( nullptr )
{
}

bool clLibrary::Load( const char* LibName )
{
#if defined( OS_WINDOWS )
	FLibHandle = ( void* )::LoadLibrary( LibName );

#else
	const char* ErrStr;

	FLibHandle = dlopen( LibName, RTLD_LAZY );

	if ( ( ErrStr = dlerror() ) != nullptr )
	{
		FLibHandle = nullptr;
	}

#endif
	return FLibHandle != nullptr;
}

clLibrary::~clLibrary()
{
#ifdef OS_WINDOWS
	FreeLibrary( ( HMODULE )FLibHandle );
#else

	if ( FLibHandle ) { dlclose( FLibHandle ); }

#endif
}

void* clLibrary::GetProcAddress( const char* ProcName ) const
{
#ifdef OS_WINDOWS
	FARPROC TheProc = ::GetProcAddress( ( HMODULE )FLibHandle, ProcName );

	return reinterpret_cast<void*>( TheProc );
#else
	void* Result = dlsym( FLibHandle, ProcName );

	const char* err_str = nullptr;

	if ( ( err_str = dlerror() ) != nullptr )
	{
		return nullptr;
	}

	return Result;
#endif
}
