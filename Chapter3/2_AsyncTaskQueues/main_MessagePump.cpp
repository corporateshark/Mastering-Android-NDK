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

#include <stdio.h>

#include "Event.h"
#include "Thread.h"

class clResponseThread: public iThread, public clAsyncQueue
{
public:
	virtual void Run()
	{
		// message pump
		for ( ;; )
		{
			DemultiplexEvents();
		}
	}
};

class clTestCall: public iAsyncCapsule
{
public:
	explicit clTestCall( int i )
		: id( i )
	{}
	virtual void Invoke()
	{
		printf( "Test %i\n", id );
	}

private:
	int id;
};

#ifndef _WIN32
#include <unistd.h>
#endif

/// Suspend current thread for a given amount of milliseconds
void OS_Sleep( int MilliSeconds )
{
#ifndef _WIN32
	usleep( MilliSeconds * 1000 );
#else
	Sleep( MilliSeconds );
#endif
}

class clRequestThread: public iThread
{
public:
	explicit clRequestThread( clAsyncQueue* Target )
		: FTarget( Target )
	{}

	virtual void Run()
	{
		int id = 0;

		for ( ;; )
		{
			FTarget->EnqueueCapsule( make_intrusive<clTestCall>( id++ ) );
			OS_Sleep( 500 );
		}
	}

private:
	clAsyncQueue* FTarget;
};


int main()
{
	{
		clResponseThread Responder;
		clRequestThread Requester( &Responder );

		Responder.Start();
		Requester.Start();

		for ( ;; ) {}
	}

	fflush( stdout );

	return 0;
}
