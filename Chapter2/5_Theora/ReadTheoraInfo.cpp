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

#include <string>
#include <iostream>
#include "LinkCode.h"

// ogg/vorbis/theora variables
ogg_sync_state   OggSyncState;
ogg_page         OggPage;
ogg_stream_state VorbisStreamState;
ogg_stream_state TheoraStreamState;
//Theora State
th_info          TheoraInfo;
th_comment       TheoraComment;
th_setup_info*   TheoraSetup;
th_dec_ctx*      TheoraDecoder;
//Vorbis State
vorbis_info      VorbisInfo;
vorbis_dsp_state VorbisDSPState;
vorbis_block     VorbisBlock;
vorbis_comment   VorbisComment;

#include <fstream>
#include <iterator>
#include <vector>

std::ifstream input( "test.ogv", std::ios::binary );

int Stream_Read( char* OutBuffer, int Size )
{
	input.read( OutBuffer, Size );

	return input.gcount();
}

int Stream_Seek( int Offset )
{
	input.seekg( Offset );

	return ( int )input.tellg();
}

int Stream_Size()
{
	input.seekg ( 0, input.end );
	int length = input.tellg();
	input.seekg ( 0, input.beg );

	return length;
}

int NumTheoraStreams, NumVorbisStreams;
int Width, Height;

float FPS, Duration;
int NumFrames;

void Theora_Cleanup()
{
	if ( TheoraDecoder )
	{
		TH_decode_free( TheoraDecoder );
		TH_setup_free( TheoraSetup );

		VORBIS_dsp_clear( &VorbisDSPState );
		VORBIS_block_clear( &VorbisBlock );

		OGG_stream_clear( &TheoraStreamState );
		TH_comment_clear( &TheoraComment );
		TH_info_clear( &TheoraInfo );

		OGG_stream_clear( &VorbisStreamState );
		VORBIS_comment_clear( &VorbisComment );
		VORBIS_info_clear( &VorbisInfo );

		OGG_sync_clear( &OggSyncState );
	}
}

bool Theora_Load()
{
	Stream_Seek( 0 );

	ogg_packet TempOggPacket;

	// init Vorbis/Theora Layer
	// Ensure all structures get cleared out.
	memset( &VorbisStreamState, 0, sizeof( ogg_stream_state ) );
	memset( &TheoraStreamState, 0, sizeof( ogg_stream_state ) );
	memset( &OggSyncState,   0, sizeof( ogg_sync_state ) );
	memset( &OggPage,        0, sizeof( ogg_page ) );
	memset( &TheoraInfo,     0, sizeof( th_info ) );
	memset( &TheoraComment,  0, sizeof( th_comment ) );
	memset( &VorbisInfo,     0, sizeof( vorbis_info ) );
	memset( &VorbisDSPState, 0, sizeof( vorbis_dsp_state ) );
	memset( &VorbisBlock,    0, sizeof( vorbis_block ) );
	memset( &VorbisComment,  0, sizeof( vorbis_comment ) );

	OGG_sync_init   ( &OggSyncState );
	TH_comment_init ( &TheoraComment );
	TH_info_init    ( &TheoraInfo );
	VORBIS_info_init( &VorbisInfo );
	VORBIS_comment_init( &VorbisComment );

	bool Done = false;

	while ( !Done )
	{
		char* Buffer = OGG_sync_buffer( &OggSyncState, 4096 );

		int BytesRead = ( int )Stream_Read( Buffer, 4096 );

		OGG_sync_wrote( &OggSyncState, BytesRead );

		if ( BytesRead == 0 )
		{
			break;
		}

		while ( OGG_sync_pageout( &OggSyncState, &OggPage ) > 0 )
		{
			ogg_stream_state OggStateTest;

			//is this an initial header? If not, stop
			if ( !OGG_page_bos( &OggPage ) )
			{
				//This is done blindly, because stream only accept themselves
				if ( NumTheoraStreams > 0 ) { OGG_stream_pagein( &TheoraStreamState, &OggPage ); }

				if ( NumVorbisStreams > 0 ) { OGG_stream_pagein( &VorbisStreamState, &OggPage ); }

				Done = true;
				break;
			}

			OGG_stream_init( &OggStateTest, OGG_page_serialno( &OggPage ) );
			OGG_stream_pagein( &OggStateTest, &OggPage );
			OGG_stream_packetout( &OggStateTest, &TempOggPacket );

			//identify the codec
			int Ret;

			if ( NumTheoraStreams == 0 )
			{
				Ret = TH_decode_headerin( &TheoraInfo, &TheoraComment, &TheoraSetup, &TempOggPacket );

				if ( Ret > 0 )
				{
					//This is the Theora Header
					memcpy( &TheoraStreamState, &OggStateTest, sizeof( OggStateTest ) );
					NumTheoraStreams = 1;
					continue;
				}
			}

			if ( NumVorbisStreams == 0 )
			{
				Ret = VORBIS_synthesis_headerin( &VorbisInfo, &VorbisComment, &TempOggPacket );

				if ( Ret >= 0 )
				{
					//This is vorbis header
					memcpy( &VorbisStreamState, &OggStateTest, sizeof( OggStateTest ) );
					NumVorbisStreams = 1;
					continue;
				}
			}

			//Hmm. I guess it's not a header we support, so erase it
			OGG_stream_clear( &OggStateTest );
		}
	}

	while ( ( ( NumTheoraStreams > 0 ) && ( NumTheoraStreams < 3 ) ) || ( ( NumVorbisStreams > 0 ) && ( NumVorbisStreams < 3 ) ) )
	{
		//Check 2nd'dary headers... Theora First
		int Success;

		while ( ( NumTheoraStreams > 0 ) && ( NumTheoraStreams < 3 ) && ( Success = OGG_stream_packetout( &TheoraStreamState, &TempOggPacket ) ) )
		{
			if ( Success < 0 )
			{
				// Error parsing Theora stream headers
				return false;
			}

			if ( !TH_decode_headerin( &TheoraInfo, &TheoraComment, &TheoraSetup, &TempOggPacket ) )
			{
				// invalid theora stream
				return false;
			}

			++NumTheoraStreams;
		} //end while looking for more theora headers

		//look 2nd vorbis header packets
		while ( NumVorbisStreams < 3 && ( Success = OGG_stream_packetout( &VorbisStreamState, &TempOggPacket ) ) )
		{
			if ( Success < 0 )
			{
				// Error parsing vorbis stream headers
				return false;
			}

			if ( VORBIS_synthesis_headerin( &VorbisInfo, &VorbisComment, &TempOggPacket ) )
			{
				// Invalid Vorbis stream
				return false;
			}

			++NumVorbisStreams;
		} //end while looking for more vorbis headers

		//Not finished with Headers, get some more file data
		if ( OGG_sync_pageout( &OggSyncState, &OggPage ) > 0 )
		{
			if ( NumTheoraStreams > 0 ) { OGG_stream_pagein( &TheoraStreamState, &OggPage ); }

			if ( NumVorbisStreams > 0 ) { OGG_stream_pagein( &VorbisStreamState, &OggPage ); }
		}
		else
		{
			char* Buffer = OGG_sync_buffer( &OggSyncState, 4096 );
			int BytesRead = ( int )Stream_Read( Buffer, 4096 );
			OGG_sync_wrote( &OggSyncState, BytesRead );

			if ( BytesRead == 0 )
			{
				// Premature end of file
				return false;
			}
		}
	} //end while looking for all headers

	//// Now we read the frame dimensions and audio parameters
	TheoraDecoder = TH_decode_alloc( &TheoraInfo, TheoraSetup );

	Width  = TheoraInfo.frame_width;
	Height = TheoraInfo.frame_height;

	return true;
}

int main()
{
	std::cout << LinkToTheora() << std::endl;

	if ( Theora_Load() ) { std::cout << "Width = " << Width << ", Height = " << Height << std::endl; }

	return 0;
}
