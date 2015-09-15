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

#pragma once

#include <map>
#include <vector>

#include "Blob.h"
#include "Streams.h"

/// Encapsulation of .zip archive management
class clArchiveReader: public iIntrusiveCounter
{
public:
	clArchiveReader()
		: FFileInfos()
		, FRealFileNames()
		, FFileInfoIdx()
		, FSourceFile()
	{}
	virtual ~clArchiveReader()
	{
		CloseArchive();
	}

	/// Assign the source stream and set the internal ownership flag
	bool OpenArchive( const clPtr<iIStream>& Source )
	{
		if ( !Source ) { return false; }

		if ( !CloseArchive() ) { return false; }

		if ( !Source->GetSize() ) { return false ; }

		FSourceFile = Source;

		return Enumerate_ZIP();
	}

	/// Extract a file from the archive into the Out stream
	bool ExtractSingleFile( const std::string& FileName, const std::string& Password, std::atomic<int>* AbortFlag, std::atomic<float>* Progress, const clPtr<iOStream>& Out );

	/// Free everything and optionally close the source stream
	bool CloseArchive();

	clPtr<iIStream> GetSourceFile() const { return FSourceFile; }

	/// Check if such a file exists in the archive
	bool FileExists( const std::string& FileName ) const { return ( GetFileIdx( FileName ) > -1 ); }

	/// Get the size of file
	uint64_t GetFileSize( const std::string& FileName ) const
	{
		return GetFileSizeIdx( GetFileIdx( FileName ) );
	}

	/// Get the size of file
	uint64_t GetFileSizeIdx( int Idx ) const
	{
		return ( Idx > -1 ) ? FFileInfos[ Idx ].FSize : 0;
	}

	/// Get the data for the file
	const void* GetFileDataIdx( int Idx )
	{
		if ( Idx <= -1 ) { return nullptr; }

		/// Check if we already have this data in cache
		if ( FExtractedFromArchive.count( Idx ) > 0 )
		{
			return FExtractedFromArchive[Idx]->GetDataConst();
		}

		/// Decompress/extract the data
		auto Blob = GetFileData_ZIP( Idx );

		if ( Blob )
		{
			FExtractedFromArchive[Idx] = Blob;

			return Blob->GetDataConst();
		}

		return nullptr;
	}

	/// Get the data for the file
	const void* GetFileData( const std::string& FileName )
	{
		return GetFileDataIdx( GetFileIdx( FileName ) );
	}

	/// Convert file name to an internal linear index
	int GetFileIdx( const std::string& FileName ) const
	{
		return ( FFileInfoIdx.count( FileName ) > 0 ) ? FFileInfoIdx[ FileName ] : -1;
	}

	/// Get the number of files in archive
	size_t GetNumFiles() const
	{ return FFileInfos.size(); }

	/// Get i-th file name in archive
	std::string GetFileName( int Idx ) const
	{ return FFileNames[ Idx ]; }

private:
	/// Internal function to enumerate the files in archive
	bool Enumerate_ZIP();

	clPtr<clBlob> GetFileData_ZIP( int Idx );

	/// Remove each extracted file from tmp container
	void ClearExtracted()
	{
		FExtractedFromArchive.clear();
	}

private:
	/// Internal file information
	struct sFileInfo
	{
		/// Offset to the file
		uint64_t FOffset;
		/// (Uncompressed) File size
		uint64_t FSize;
		/// Compressed file size
		uint64_t FCompressedSize;
		/// Compressed data
		void* FSourceData;
	};

	/// Collection of all file information within the archive
	std::vector<sFileInfo> FFileInfos;

	/// File names (adapted for our VFS - upper case, slash-separated path)
	std::vector<std::string> FFileNames;

	/// Real (in-archive) file names
	std::vector<std::string> FRealFileNames;

	/// File information indices (name-to-index map)
	mutable std::map<std::string, int> FFileInfoIdx;

	/// Source .zip file
	clPtr<iIStream> FSourceFile;

	/// Temporary cache for the extracted files. Removed on CloseArchive() call
	std::map<int, clPtr<clBlob> > FExtractedFromArchive;
};
