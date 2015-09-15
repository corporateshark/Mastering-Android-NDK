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

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/mman.h>
#  include <fcntl.h>
#  include <errno.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <vector>

#include "Streams.h"
#include "Blob.h"

class iRawFile: public iIntrusiveCounter
{
public:
	iRawFile()
		: FFileName()
		, FVirtualFileName()
	{};

	void SetVirtualFileName( const std::string& VFName )
	{ FVirtualFileName = VFName; }

	void SetFileName( const std::string& FName )
	{ FFileName = FName; }

	std::string GetVirtualFileName() const
	{ return FVirtualFileName; }

	std::string GetFileName() const
	{ return FFileName; }

	virtual const uint8_t* GetFileData() const = 0;
	virtual uint64_t       GetFileSize() const = 0;

protected:
	std::string FFileName;
	std::string FVirtualFileName;
};

class clNullRawFile: public iRawFile
{
public:
	clNullRawFile()
		: FData( 0 )
	{}

	virtual const uint8_t* GetFileData() const override
	{ return &FData; }

	virtual uint64_t       GetFileSize() const override
	{ return 0; }

private:
	uint8_t FData;
};

/// Physical file representation
class clRawFile: public iRawFile
{
public:
	clRawFile()
		: FFileData( nullptr )
		, FSize( 0 )
	{}

	virtual ~clRawFile()
	{ Close(); }

	bool Open( const std::string& FileName, const std::string& VirtualFileName )
	{
		SetFileName( FileName );
		SetVirtualFileName( VirtualFileName );

		FSize = 0;
		FFileData = nullptr;

#ifdef _WIN32
		FMapFile = CreateFileA( FFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr );
		FMapHandle = CreateFileMapping( FMapFile, nullptr, PAGE_READONLY, 0, 0, nullptr );
		FFileData = ( uint8_t* )MapViewOfFile( FMapHandle, FILE_MAP_READ, 0, 0, 0 );

		if ( !FFileData )
		{
			CloseHandle( ( HANDLE )FMapHandle );
			return false;
		}

		DWORD dwSizeLow = 0, dwSizeHigh = 0;
		dwSizeLow = ::GetFileSize( FMapFile, &dwSizeHigh );
		FSize = ( ( uint64_t )dwSizeHigh << 32 ) | ( uint64_t )dwSizeLow;
#else
		FFileHandle = open( FileName.c_str(), O_RDONLY );

		struct stat FileInfo;

		if ( !fstat( FFileHandle, &FileInfo ) )
		{
			FSize = static_cast<uint64_t>( FileInfo.st_size );

			// don't call mmap() for zero-sized files
			if ( FSize ) { FFileData = ( uint8_t* )( mmap( nullptr, FSize, PROT_READ, MAP_PRIVATE, FFileHandle, 0 ) ); }
		}

		close( FFileHandle );
#endif
		return true;
	}

	void Close()
	{
#ifdef _WIN32

		if ( FFileData  ) { UnmapViewOfFile( FFileData ); }

		if ( FMapHandle ) { CloseHandle( ( HANDLE )FMapHandle ); }

		CloseHandle( ( HANDLE )FMapFile );
#else

		if ( FFileData ) { munmap( FFileData, FSize ); }

#endif
	}

	virtual const uint8_t* GetFileData() const { return FFileData; }
	virtual uint64_t       GetFileSize() const { return FSize;     }
private:
#ifdef _WIN32
	HANDLE FMapFile;
	HANDLE FMapHandle;
#else
	int FFileHandle;
#endif
	uint8_t* FFileData;
	uint64_t FSize;
};

class clMemRawFile: public iRawFile
{
public:
	clMemRawFile()
		: FOwnsBuffer( false )
		, FBuffer( nullptr )
		, FBufferSize( 0 )
	{}

	clMemRawFile( const uint8_t* BufPtr, size_t BufSize, bool OwnsBuffer )
		: FOwnsBuffer( OwnsBuffer )
		, FBuffer( BufPtr )
		, FBufferSize( BufSize )
	{}

	virtual ~clMemRawFile()
	{ DeleteBuffer(); }

	virtual const uint8_t* GetFileData() const override
	{ return FBuffer; }

	virtual uint64_t       GetFileSize() const override
	{ return FBufferSize; }

	static clPtr<clMemRawFile> CreateFromString( const std::string& InString )
	{
		size_t   BufferSize = InString.length();
		uint8_t* Buffer     = nullptr;

		if ( BufferSize > 0 )
		{
			Buffer = new uint8_t[ InString.length() ];

			memcpy( Buffer, InString.c_str(), BufferSize );
		}

		return make_intrusive<clMemRawFile>( Buffer, BufferSize, true );
	}

	static clPtr<clMemRawFile> CreateFromBuffer( const void* BufPtr, size_t BufSize )
	{
		return make_intrusive<clMemRawFile>( reinterpret_cast<const uint8_t*>( BufPtr ), BufSize, true );
	}

	static clPtr<clMemRawFile> CreateFromManagedBuffer( const void* BufPtr, size_t BufSize )
	{
		return make_intrusive<clMemRawFile>( reinterpret_cast<const uint8_t*>( BufPtr ), BufSize, false );
	}

private:
	void DeleteBuffer()
	{
		if ( FBuffer && FOwnsBuffer )
		{
			delete[]( FBuffer );
		}

		FBuffer = nullptr;
	}

	/// Should we delete the buffer?
	bool FOwnsBuffer;

	const uint8_t* FBuffer;

	size_t FBufferSize;
};

class clManagedMemRawFile: public iRawFile
{
public:
	clManagedMemRawFile()
		: FBlob( nullptr )
	{}

	virtual const uint8_t* GetFileData() const override
	{ return ( const uint8_t* )FBlob->GetData(); }

	virtual uint64_t       GetFileSize() const override
	{ return FBlob->GetSize(); }

	void SetBlob( const clPtr<clBlob>& Ptr )
	{ FBlob = Ptr; }

private:
	clPtr<clBlob> FBlob;
};

class clFileMapper: public iIStream
{
public:
	explicit clFileMapper( const clPtr<iRawFile>& File )
		: FFile( File )
		, FPosition( 0 )
	{}

	virtual ~clFileMapper()
	{}

	virtual std::string GetVirtualFileName() const override
	{ return FFile->GetVirtualFileName(); }

	virtual std::string GetFileName() const override
	{ return FFile->GetFileName(); }

	virtual uint64_t Read( const void* Buf, uint64_t Size ) override
	{
		uint64_t RealSize = ( Size > GetBytesLeft() ) ? GetBytesLeft() : Size;

		if ( !RealSize ) { return 0; }

		memcpy( ( void* )Buf, ( FFile->GetFileData() + FPosition ), static_cast<size_t>( RealSize ) );

		FPosition += RealSize;

		return RealSize;
	}

	virtual void Seek( uint64_t Position ) override
	{ FPosition  = Position; }

	virtual uint64_t GetSize() const override
	{ return FFile->GetFileSize(); }

	virtual uint64_t GetPos() const override
	{ return FPosition; }

	virtual bool Eof() const override
	{ return ( FPosition >= FFile->GetFileSize() ); }

	virtual const uint8_t* MapStream() const override
	{ return FFile->GetFileData(); }

	virtual const uint8_t* MapStreamFromCurrentPos() const override
	{ return ( FFile->GetFileData() + FPosition ); }

private:
	clPtr<iRawFile> FFile;
	uint64_t        FPosition;
};

class clFileWriter: public iOStream
{
public:
	clFileWriter()
		: FPosition( 0 )
	{}

	virtual ~clFileWriter()
	{ Close(); }

	bool Open( const std::string& FileName )
	{
		FFileName = FileName;
#ifdef _WIN32
		FMapFile = CreateFile( FFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );

		return !( FMapFile == ( void* )INVALID_HANDLE_VALUE );
#else
		FMapFile = open( FFileName.c_str(), O_WRONLY | O_CREAT );
		FPosition = 0;
		return !( FMapFile == -1 );
#endif
	}

	void Close()
	{
#ifdef _WIN32
		CloseHandle( FMapFile );
#else

		if ( FMapFile != -1 ) { close( FMapFile ); }

#endif
	}

	virtual std::string GetFileName() const override
	{ return FFileName; }

	virtual uint64_t GetFilePos() const override
	{ return FPosition; }

	virtual void Seek( const uint64_t Position ) override
	{
#ifdef _WIN32
		SetFilePointerEx( FMapFile, *reinterpret_cast<const LARGE_INTEGER*>( &Position ), nullptr, FILE_BEGIN );
#else

		if ( FMapFile != -1 ) { lseek( FMapFile, Position, SEEK_SET ); }

#endif
		FPosition = Position;
	}

	virtual uint64_t Write( const void* Buf, uint64_t Size ) override
	{
#ifdef _WIN32
		DWORD written;
		WriteFile( FMapFile, Buf, DWORD( Size ), &written, nullptr );
#else

		if ( FMapFile != -1 ) { write( FMapFile, Buf, Size ); }

#endif
		FPosition += Size;
		return Size;
	}

private:
	std::string FFileName;
#ifdef _WIN32
	HANDLE FMapFile;
#else
	int    FMapFile;
#endif
	uint64_t    FPosition;
};

class clMemFileWriter: public iOStream
{
public:
	clMemFileWriter()
		: FBlob( make_intrusive<clBlob>() )
		, FFileName()
		, FPosition( 0 )
	{}
	explicit clMemFileWriter( const clPtr<clBlob>& Blob )
		: FBlob( Blob )
		, FFileName()
		, FPosition( 0 )
	{}

	virtual std::string GetFileName() const override
	{ return FFileName; }

	virtual uint64_t GetFilePos() const override
	{ return FPosition; }

	/// Change absolute file position to Position
	virtual void Seek( const uint64_t Position ) override
	{
		FPosition = ( Position > FBlob->GetSize() ) ? FBlob->GetSize() - 1 : Position;
	}

	/// Write Size bytes from Buf
	virtual uint64_t Write( const void* Buf, uint64_t Size ) override
	{
		return FBlob->AppendBytes( Buf, static_cast<size_t>( Size ) );
	}

	virtual void Reserve( uint64_t Size ) override
	{ FBlob->Reserve( static_cast<size_t>( Size ) ); };

	clPtr<clBlob> GetBlob() const
	{ return FBlob; }

	//
	// clMemFileWriter
	//

	virtual void SetFileName( const std::string& FName )
	{
		FFileName = FName;
	}

	/// Access internal data container
	const uint8_t* GetBuffer() const
	{
		return static_cast<const uint8_t*>( FBlob->GetDataConst() );
	}

private:
	/// Actual file contents
	clPtr<clBlob> FBlob;

	/// Virtual file name
	std::string FFileName;

	/// Current position
	uint64_t FPosition;
};
