/*
 * Copyright (C) 2013 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013 Viktor Latypov (vl@linderdaum.com)
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

#include "Decoders.h"
#include "DecodingProvider.h"

#include "minimp3.h"

#include <stdio.h>

class clMP3Provider: public iDecodingProvider
{
public:
	clMP3Provider( const clPtr<clBlob>& Blob )
		: iDecodingProvider( Blob )
	{
		FBuffer.resize( MP3_MAX_SAMPLES_PER_FRAME * 8 );
		FBufferUsed = 0;

		FBitsPerSample = 16;

		mp3 = mp3_create();

		bytes_left = ( int )FRawData->GetSize();

		stream_pos = 0;

		byte_count = mp3_decode( ( mp3_decoder_t* )mp3, ( void* )FRawData->GetData(), bytes_left, ( signed short* )&FBuffer[0], &info );
		bytes_left -= byte_count;
		stream_pos = 0; // += byte_count;

		FSamplesPerSec = info.sample_rate;
		FChannels = info.channels;
	}
	virtual ~clMP3Provider()
	{
		mp3_done( &mp3 );
	}

	virtual int ReadFromFile( int Size, int BytesRead )
	{
		byte_count = mp3_decode( ( mp3_decoder_t* )mp3,
		                         ( ( char* )FRawData->GetData() ) + stream_pos,
		                         bytes_left, ( signed short* )( &FBuffer[0] + BytesRead ), &info );

		bytes_left -= byte_count;
		stream_pos += byte_count;

		return info.audio_bytes;
	}

	virtual void Seek( float Time )
	{
		// use info.sample_rate ?
		// theoreticaly we can recalculate Time to bytes, using bitrate... and set the stream_pos

		FEof = false;
	}

private:
	mp3_decoder_t mp3;
	mp3_info_t info;

	int stream_pos;
	int bytes_left;
	int byte_count;
};
