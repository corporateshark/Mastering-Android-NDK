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

#include <stdio.h>
#include <fstream>
#include <vector>

#include "FileSystem.h"
#include "Blob.h"

#include "Thread.h"
#include "LAL.h"

clPtr<clFileSystem> g_FS;

#pragma pack(push, 1)
struct __attribute__( ( packed, aligned( 1 ) ) ) sWAVHeader
{
	unsigned char    RIFF[4];
	unsigned long    Size;
	unsigned char    WAVE[4];
	unsigned char    FMT[4];
	unsigned int     SizeFmt;
	unsigned short   FormatTag;
	unsigned short   Channels;
	unsigned int     SampleRate;
	unsigned int     AvgBytesPerSec;
	unsigned short   nBlockAlign;
	unsigned short   nBitsperSample;
	unsigned char    Reserved[4];
	unsigned long    DataSize;
};
#pragma pack(pop)

class clSoundThread: public iThread
{
	ALuint FSourceID;
	ALuint FBufferID;

	ALCdevice*  FDevice;
	ALCcontext* FContext;

	bool IsPlaying() const
	{
		int State;
		alGetSourcei( FSourceID, AL_SOURCE_STATE, &State );
		return State == AL_PLAYING;
	}

	void CreateBuffer()
	{
		alGenBuffers( 1, &FBufferID );
	}

	void PlayBuffer( const unsigned char* Data, int DataSize, int SampleRate )
	{
		alBufferData( FBufferID, AL_FORMAT_MONO16, Data, DataSize, SampleRate );
		alSourcei( FSourceID, AL_BUFFER, FBufferID );
		alSourcei( FSourceID, AL_LOOPING, AL_FALSE );
		alSourcef( FSourceID, AL_GAIN, 1.0f );
		alSourcePlay( FSourceID );
	}

	virtual void Run()
	{
		LoadAL();

		// We should use actual device name if the default does not work
		FDevice = alcOpenDevice( nullptr );
		FContext = alcCreateContext( FDevice, nullptr );
		alcMakeContextCurrent( FContext );

		// create the source
		alGenSources( 1, &FSourceID );
		alSourcef( FSourceID, AL_GAIN,    1.0 );

		// load data
		auto Blob = LoadFileAsBlob( g_FS, "test.wav" );

		const sWAVHeader* Header = ( const sWAVHeader* )Blob->GetData();

		const unsigned char* WaveData = ( const unsigned char* )Blob->GetData() + sizeof( sWAVHeader );

		// prepare buffer
		CreateBuffer();

		// play
		PlayBuffer( WaveData, Header->DataSize, Header->SampleRate );

		while ( IsPlaying() ) { }

		alSourceStop( FSourceID );

		Sleep( 500 );

		// destroy the source
		alDeleteSources( 1, &FSourceID );
		alDeleteBuffers( 1, &FBufferID );

		// Shutdown
		alcDestroyContext( FContext );
		alcCloseDevice( FDevice );

		UnloadAL();
	}
};

clSoundThread g_Sound;

int main()
{
	g_FS = make_intrusive<clFileSystem>();
	g_FS->Mount( "." );

	g_Sound.Start();
	g_Sound.Exit( true );

	return 0;
}
