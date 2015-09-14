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
#include <algorithm>

#include "CurlWrap.h"
#include "Downloader.h"
#include "Blob.h"
#include "Event.h"
#include "urlparser/LUrlParser.h"

std::atomic<bool> g_ShouldExit( false );

std::string BaseName( const std::string& Path )
{
	return std::string( std::find_if( Path.rbegin(), Path.rend(), []( char c ) { return c == '/'; } ).base(), Path.end() );
}

class clTestCallback: public clDownloadCompleteCallback
{
public:
	clTestCallback(): FPrevDownloaded( 0 ) {}

	virtual void Progress( double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded ) override
	{
		constexpr double Mega = 1024.0 * 1024.0;

		int Downloaded = int( NowDownloaded / Mega );

		if ( TotalToDownload && Downloaded != FPrevDownloaded )
		{
			printf( "Downloaded %i Mb of %i Mb\n", int( Downloaded ), int( TotalToDownload / Mega ) );

			FPrevDownloaded = Downloaded;
		}
	}

	virtual void Invoke() override
	{
		if ( FResult && FTask )
		{
			printf( "Download complete\n" );

			LUrlParser::clParseURL URL = LUrlParser::clParseURL::ParseURL( FTask->GetURL() );

			if ( URL.IsValid() )
			{
				printf( "URL = %s\n", FTask->GetURL().c_str() );
				printf( "File = %s\n", BaseName( URL.m_Path ).c_str() );

				FILE* File = fopen( BaseName( URL.m_Path ).c_str(), "wb" );

				fwrite( FResult->GetDataConst(), FResult->GetSize(), 1, File );

				fclose( File );
			}
		}
		else
		{
			printf( "Download failed: %i\n", int( FCurlCode ) );
		}

		g_ShouldExit = true;
	}

private:
	int FPrevDownloaded;
};

int main()
{
	Curl_Load();

	auto Events = make_intrusive<clAsyncQueue>();

	auto Downloader = make_intrusive<clDownloader>( Events );

	clPtr<clDownloadTask> Task = Downloader->DownloadURL( "http://downloads.sourceforge.net/freeimage/FreeImage3160.zip", 1, make_intrusive<clTestCallback>() );

	while ( !g_ShouldExit )
	{
		// dispatch events here
		Events->DemultiplexEvents();
	}

	return 0;
}
