#include <algorithm>

#include "Audio.h"

void clAudioThread::RegisterSource( clAudioSource* Src )
{
	tthread::lock_guard<tthread::mutex> Lock( FMutex );

	auto i = std::find( FActiveSources.begin(), FActiveSources.end(), Src );

	if ( i != FActiveSources.end() ) { return; }

	FActiveSources.push_back( Src );
}

void clAudioThread::UnRegisterSource( clAudioSource* Src )
{
	tthread::lock_guard<tthread::mutex> Lock( FMutex );

	auto i = std::find( FActiveSources.begin(), FActiveSources.end(), Src );

	if ( i != FActiveSources.end() ) { FActiveSources.erase( i ); }
}

void clAudioThread::Run()
{
	if ( !LoadAL() ) { return; }

	// We should use actual device name if the default does not work
	FDevice = alcOpenDevice( nullptr );

	FContext = alcCreateContext( FDevice, nullptr );

	alcMakeContextCurrent( FContext );

	FInitialized = true;

	double Seconds = Env_GetSeconds();

	while ( !IsPendingExit() )
	{
		float DeltaSeconds = static_cast<float>( Env_GetSeconds() - Seconds );

		{
			tthread::lock_guard<tthread::mutex> Lock( FMutex );

			for ( auto i = FActiveSources.begin(); i != FActiveSources.end(); i++ )
			{
				( *i )->Update( DeltaSeconds );
			}
		}

		Seconds = Env_GetSeconds();

		Env_Sleep( 100 );
	}

	alcDestroyContext( FContext );
	alcCloseDevice( FDevice );

	UnloadAL();
}

void clAudioThread::Wait() const volatile
{
	while ( !FInitialized ) {};
}
