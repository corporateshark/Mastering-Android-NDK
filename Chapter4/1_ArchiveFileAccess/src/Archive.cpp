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
#include <stdint.h>

#include "Archive.h"
#include "Files.h"
#include "FileSystem.h"
#include "MountPoint.h"

#include "libcompress.h"

// dummy error handler for the bzip2 library
extern "C" void bz_internal_error( int e_code ) { ( void )e_code; }

bool clArchiveReader::CloseArchive()
{
	/// Clear containers
	FFileInfos.clear();
	FFileInfoIdx.clear();
	FFileNames.clear();
	FRealFileNames.clear();
	ClearExtracted();

	FSourceFile = nullptr;

	return true;
}

static voidpf ZCALLBACK zip_fopen ( voidpf opaque, const void* filename, int mode )
{
	( ( iIStream* )opaque )->Seek( 0 );
	( void )filename;
	( void )mode;
	return opaque;
}

static uLong ZCALLBACK zip_fread ( voidpf opaque, voidpf stream, void* buf, uLong size )
{
	iIStream* S = ( iIStream* )stream;
	int64_t CanRead = ( int64_t )size;
	int64_t Sz = S->GetSize();
	int64_t Ps = S->GetPos();

	if ( CanRead + Ps >= Sz ) { CanRead = Sz - Ps; }

	if ( CanRead > 0 ) {  S->Read( buf, ( uint64_t )CanRead ); }
	else { CanRead = 0; }

	return ( uLong )CanRead;
}

static ZPOS64_T ZCALLBACK zip_ftell ( voidpf opaque, voidpf stream )
{
	return ( ZPOS64_T )( ( iIStream* )stream )->GetPos();
}

static long ZCALLBACK zip_fseek ( voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin )
{
	iIStream* S = ( iIStream* )stream;
	int64_t NewPos = ( int64_t )offset;
	int64_t Sz = ( int64_t )S->GetSize();

	switch ( origin )
	{
		case ZLIB_FILEFUNC_SEEK_CUR:
			NewPos += ( int64_t )S->GetPos();
			break;

		case ZLIB_FILEFUNC_SEEK_END:
			NewPos = Sz - 1 - NewPos;
			break;

		case ZLIB_FILEFUNC_SEEK_SET:
			break;

		default:
			return -1;
	}

	if ( NewPos >= 0 && ( NewPos < Sz ) ) { S->Seek( ( uint64_t )NewPos ); }
	else { return -1; }

	return 0;
}

static int ZCALLBACK zip_fclose ( voidpf opaque, voidpf stream ) { return 0; }
static int ZCALLBACK zip_ferror ( voidpf opaque, voidpf stream ) { return 0; }

void fill_functions( iIStream* Stream, zlib_filefunc64_def*  pzlib_filefunc_def )
{
	pzlib_filefunc_def->zopen64_file = zip_fopen;
	pzlib_filefunc_def->zread_file = zip_fread;
	pzlib_filefunc_def->zwrite_file = NULL;
	pzlib_filefunc_def->ztell64_file = zip_ftell;
	pzlib_filefunc_def->zseek64_file = zip_fseek;
	pzlib_filefunc_def->zclose_file = zip_fclose;
	pzlib_filefunc_def->zerror_file = zip_ferror;
	pzlib_filefunc_def->opaque = Stream;
}

/*
   .ZIP reading code is based on the following source:

   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )
   Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )
   Copyright (C) 2007-2008 Even Rouault
   Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#define WRITEBUFFERSIZE (8192)

/// Internal routine to extract a single file from ZIP archive
int ExtractCurrentFile_ZIP( unzFile UnzipFile, const char* Password, std::atomic<int>* AbortFlag, std::atomic<float>* Progress, const clPtr<iOStream>& Out )
{
	char FilenameInzip[1024];
	unz_file_info64 FileInfo;

	int ErrorCode = unzGetCurrentFileInfo64( UnzipFile, &FileInfo, FilenameInzip, sizeof( FilenameInzip ), nullptr, 0, nullptr, 0 );

	if ( ErrorCode != UNZ_OK ) { return ErrorCode; }

	ErrorCode = unzOpenCurrentFilePassword( UnzipFile, Password );

	if ( ErrorCode != UNZ_OK ) { return ErrorCode; }

	uint64_t FileSize = ( uint64_t )FileInfo.uncompressed_size;

	Out->Reserve( ( size_t )FileSize );

	unsigned char Buffer[ WRITEBUFFERSIZE ];

	uint64_t TotalBytes = 0;

	int BytesRead = 0;

	do
	{
		if ( AbortFlag && *AbortFlag ) { break; }

		BytesRead = unzReadCurrentFile( UnzipFile, Buffer, WRITEBUFFERSIZE );

		if ( BytesRead < 0 ) { break; }

		if ( BytesRead > 0 )
		{
			TotalBytes += BytesRead;
			Out->Write( Buffer, BytesRead );
		}

		if ( Progress )
		{
			*Progress = ( float )TotalBytes / ( float )FileSize;
		}
	}
	while ( BytesRead > 0 );

	ErrorCode = unzCloseCurrentFile( UnzipFile );

	return ErrorCode;
}

bool clArchiveReader::Enumerate_ZIP()
{
	clPtr<iIStream> TheSource = FSourceFile;
	FSourceFile->Seek( 0 );

	zlib_filefunc64_def ffunc;
	fill_functions( TheSource.GetInternalPtr(), &ffunc );

	unzFile UnzipFile = unzOpen2_64( "", &ffunc );

	unz_global_info64 gi;

	int ErrorCode = unzGetGlobalInfo64( UnzipFile, &gi );

	for ( uLong i = 0; i < gi.number_entry; i++ )
	{
		if ( ErrorCode != UNZ_OK ) { break; }

		char filename_inzip[256];
		unz_file_info64 file_info;

		ErrorCode = unzGetCurrentFileInfo64( UnzipFile, &file_info, filename_inzip, sizeof( filename_inzip ), nullptr, 0, nullptr, 0 );

		if ( ErrorCode != UNZ_OK ) { break; }

		if ( ( i + 1 ) < gi.number_entry )
		{
			ErrorCode = unzGoToNextFile( UnzipFile );

			// WARNING: "error %d with zipfile in unzGoToNextFile\n", err
			if ( ErrorCode != UNZ_OK ) { break; }
		}

		sFileInfo Info;
		Info.FOffset = 0;
		Info.FCompressedSize = file_info.compressed_size;
		Info.FSize = file_info.uncompressed_size;
		FFileInfos.push_back( Info );

		std::string TheName = Arch_FixFileName( filename_inzip );
		FFileInfoIdx[ TheName ] = ( int )FFileNames.size();
		FFileNames.push_back( TheName );
		FRealFileNames.emplace_back( filename_inzip );
	}

	unzClose( UnzipFile );

	return true;
}

/// end of .ZIP stuff

bool clArchiveReader::ExtractSingleFile( const std::string& FileName, const std::string& Password, std::atomic<int>* AbortFlag, std::atomic<float>* Progress, const clPtr<iOStream>& Out )
{
	std::string ZipName = FileName;
	std::replace( ZipName.begin(), ZipName.end(), '\\', '/' );

	clPtr<iIStream> TheSource = FSourceFile;
	FSourceFile->Seek( 0 );

	/// Decompress the data
	zlib_filefunc64_def ffunc;
	fill_functions( TheSource.GetInternalPtr(), &ffunc );

	unzFile UnzipFile = unzOpen2_64( "", &ffunc );

	if ( unzLocateFile( UnzipFile, ZipName.c_str( ), 0/*CASESENSITIVITY - insensitive*/ ) != UNZ_OK )
	{
		// WARNING: "File %s not found in the zipfile\n", FName.c_str()
		return false;
	}

	int ErrorCode = ExtractCurrentFile_ZIP( UnzipFile, Password.empty( ) ? nullptr : Password.c_str( ), AbortFlag, Progress, Out );

	unzClose( UnzipFile );

	return ( ErrorCode == UNZ_OK );
}

clPtr<clBlob> clArchiveReader::GetFileData_ZIP( int Idx )
{
	if ( FExtractedFromArchive.count( Idx ) > 0 )
	{
		return FExtractedFromArchive[ Idx ];
	}

	clPtr<clMemFileWriter> Out = clFileSystem::CreateMemWriter( "mem_blob", FFileInfos[ Idx ].FSize );

	if ( ExtractSingleFile( FRealFileNames[ Idx ], "", nullptr, nullptr, Out ) )
	{
		return Out->GetBlob();
	}


	return make_intrusive<clBlob>();
}
