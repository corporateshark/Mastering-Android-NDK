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

#include "Timers.h"

#if defined(ANDROID) || defined( OS_EMSCRIPTEN )
#  include <thread>
#  include <chrono>
#  include <sys/time.h>
#endif

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <time.h>

void Env_Sleep( int Milliseconds )
{
#if defined(_WIN32)
	Sleep( Milliseconds );
#elif defined(ANDROID)
	std::this_thread::sleep_for( std::chrono::milliseconds( Milliseconds ) );
#else
	// mu-sleep supports microsecond-precision
	usleep( static_cast<useconds_t>( Milliseconds ) * 1000 );
#endif
}

double Env_GetSeconds()
{
#if defined(_WIN32)
	return ( double )clock() / CLOCKS_PER_SEC;
#else
	const unsigned usec_per_sec = 1000000;
	const double RecipCyclesPerSecond = 1.0 / usec_per_sec;

	struct timeval Time;

	gettimeofday( &Time, NULL );
	int64_t performance_count = Time.tv_usec + Time.tv_sec * usec_per_sec;

	return RecipCyclesPerSecond * ( double )performance_count;
#endif
}
