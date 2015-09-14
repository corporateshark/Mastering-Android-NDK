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

#include "Thread.h"

THREAD_CALL iThread::EntryPoint( void* Ptr )
{
	iThread* Thread = reinterpret_cast<iThread*>( Ptr );

	if ( Thread )
	{
		Thread->Run();
	}

#ifdef _WIN32
	_endthreadex( 0 );
	return 0;
#else
	pthread_exit( 0 );
	return nullptr;
#endif
}

void iThread::SetPriority( LPriority Priority )
{
#ifdef _WIN32
	int P = THREAD_PRIORITY_IDLE;

	switch ( Priority )
	{
		case Priority_Lowest:
			P = THREAD_PRIORITY_LOWEST;
			break;

		case Priority_Low:
			P = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case Priority_Normal:
			P = THREAD_PRIORITY_NORMAL;
			break;

		case Priority_High:
			P = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case Priority_Highest:
			P = THREAD_PRIORITY_HIGHEST;
			break;

		case Priority_TimeCritical:
			P = THREAD_PRIORITY_TIME_CRITICAL;
			break;
	}

	SetThreadPriority( ( HANDLE )FThreadHandle, P );
#else
	int SchedPolicy = SCHED_OTHER;

	int MaxP = sched_get_priority_max( SchedPolicy );
	int MinP = sched_get_priority_min( SchedPolicy );

	sched_param SchedParam;
	SchedParam.sched_priority = MinP + ( MaxP - MinP ) / ( Priority_TimeCritical - Priority + 1 );

	pthread_setschedparam( FThreadHandle, SchedPolicy, &SchedParam );
#endif
}

native_thread_handle_t iThread::GetCurrentThread()
{
#if defined( _WIN32)
	return GetCurrentThreadId();
#elif defined( ANDROID )
	return gettid();
#else
	return pthread_self();
#endif
}
