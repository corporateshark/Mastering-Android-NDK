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
#include "LAL.h"
#include "Audio.h"
#include "FileSystem.h"
#include "Blob.h"
#include "MOD.h"
#include "Callbacks.h"
#include "Log.h"

clPtr<clFileSystem> g_FS;

clAudioThread g_Audio;

class clSoundThread: public iThread
{
	virtual void Run()
	{
		g_Audio.WaitForInitialization();

		auto Src = make_intrusive<clAudioSource>();

		// http://modarchive.org/index.php?request=view_by_moduleid&query=172184
		// Public domain
		auto Blob = LoadFileAsBlob( g_FS, "augmented_emotions.xm" );

		if ( Blob->GetSize() == 0 )
		{
			printf( "Invalid module data file" );
			return;
		}

		Src->BindWaveform( make_intrusive<clModPlugProvider>( Blob ) );
		Src->Play();

		double Seconds = Env_GetSeconds();

		while ( !IsPendingExit() )
		{
			float DeltaSeconds = static_cast<float>( Env_GetSeconds() - Seconds );
			Src->Update( DeltaSeconds );
			Seconds = Env_GetSeconds();
		}

		Src->Stop();

		Env_Sleep( 500 );
	}
};

clSoundThread g_Sound;

void OnStart( const std::string& RootPath )
{
	LoadAL();
	LoadModPlug();

	g_FS = make_intrusive<clFileSystem>();
	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );

	g_Audio.Start();
	g_Sound.Start();

}

void OnStop()
{
	g_Sound.Exit( true );
	g_Audio.Exit( true );
}

#if !defined(ANDROID)

#include <conio.h>

int main()
{
	OnStart( "" );

	while ( !kbhit() ) {};

	OnStop();

	return 0;
}

#endif // ANDROID
