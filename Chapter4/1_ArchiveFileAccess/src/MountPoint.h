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

#include <sys/stat.h>
#include <algorithm>

#include "Archive.h"
#include "Files.h"
#include "StrUtils.h"

#if defined( _WIN32 )
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif // _WIN32

inline std::string Arch_FixFileName( const std::string& VName )
{
	std::string s( VName );
	std::replace( s.begin(), s.end(), '\\', PATH_SEPARATOR );
	std::replace( s.begin(), s.end(), '/', PATH_SEPARATOR );
	return s;
}

/// Mount point interface for virtual file system
class iMountPoint: public iIntrusiveCounter
{
public:
	iMountPoint()
		: FName()
	{}

	virtual ~iMountPoint()
	{}

	/// Check if virtual file VirtualName exists at this mount point
	virtual bool         FileExists( const std::string& VirtualName ) const = 0;

	/// Convert local file VirtualName to global name
	virtual std::string      MapName( const std::string& VirtualName ) const = 0;

	/// Create appropriate file reader for the specified VirtualName
	virtual clPtr<iRawFile>  CreateReader( const std::string& VirtualName ) const = 0;

	/// Set internal mount point name
	virtual void    SetName( const std::string& N ) { FName = N; }

	/// Get internal mount point name
	virtual std::string GetName() const { return FName; }

private:
	std::string FName;
};

inline bool FS_IsFullPath( const std::string& Path )
{
	return ( Path.find( ":\\" ) != std::string::npos ||
#if !defined( _WIN32 )
	         ( Path.length() && Path[0] == '/' ) ||
#endif
	         Path.find( ":/" )  != std::string::npos ||
	         Path.find( ".\\" ) != std::string::npos );
}

inline std::string FS_ValidatePath( const std::string& PathName )
{
	std::string Result = PathName;

	for ( size_t i = 0; i != Result.length(); ++i )
	{
		if ( Result[i] == '/' || Result[i] == '\\' )
		{
			Result[i] = PATH_SEPARATOR;
		}
	}

	return Result;
}

inline bool FS_FileExistsPhys( const std::string& PhysicalName )
{
#ifdef _WIN32
	struct _stat buf;
	int Result = _stat( FS_ValidatePath( PhysicalName ).c_str(), &buf );
#else
	struct stat buf;
	int Result = stat( FS_ValidatePath( PhysicalName ).c_str(), &buf );
#endif
	return Result == 0;
}

/// Mount point implementation for the physical folder
class clPhysicalMountPoint: public iMountPoint
{
public:
	explicit clPhysicalMountPoint( const std::string& PhysicalName )
		: FPhysicalName( PhysicalName )
	{
		Str::AddTrailingChar( &FPhysicalName, PATH_SEPARATOR );
	}

	virtual bool FileExists( const std::string& VirtualName ) const override
	{ return FS_FileExistsPhys( MapName( VirtualName ) ); }

	virtual std::string MapName( const std::string& VirtualName ) const override
	{
		return FS_IsFullPath( VirtualName ) ? VirtualName : ( FPhysicalName + VirtualName );
	}

	virtual clPtr<iRawFile> CreateReader( const std::string& VirtualName ) const override
	{
		std::string PhysName = MapName( VirtualName );

		auto File = make_intrusive<clRawFile>();

		if ( File->Open( FS_ValidatePath( PhysName ), VirtualName ) ) { return File; }

		return make_intrusive<clNullRawFile>();
	}

private:
	std::string FPhysicalName;
};

/// The decorator to allow file name dereferencing
class clAliasMountPoint: public iMountPoint
{
public:
	explicit clAliasMountPoint( const clPtr<iMountPoint>& Src )
		: FAlias()
		, FMP( Src )
	{}

	virtual bool FileExists( const std::string& VirtualName ) const override
	{
		return FMP->FileExists( FAlias + VirtualName );
	}

	virtual std::string MapName( const std::string& VirtualName ) const override
	{ return FMP->MapName( FAlias + VirtualName ); }

	virtual clPtr<iRawFile> CreateReader( const std::string& VirtualName ) const override
	{ return FMP->CreateReader( FAlias + VirtualName ); }

	//
	// clAliasMountPoint
	//

	/// Set the alias directory
	void SetAlias( const std::string& N )
	{
		FAlias = N;
		Str::AddTrailingChar( &FAlias, PATH_SEPARATOR );
	}

	/// Get internal mount point name
	std::string GetAlias() const
	{ return FAlias; }

private:
	/// Name to append to each file in this mount point
	std::string FAlias;

	/// The actual file container
	clPtr<iMountPoint> FMP;
};

/// Implementation of a mount point for the .zip files
class clArchiveMountPoint: public iMountPoint
{
public:
	explicit clArchiveMountPoint( const clPtr<clArchiveReader>& R )
		: FReader( R )
	{}

	virtual clPtr<iRawFile> CreateReader( const std::string& VirtualName ) const override
	{
		std::string Name = Arch_FixFileName( VirtualName );

		const void* DataPtr  = FReader->GetFileData( Name );
		size_t      FileSize = static_cast<size_t>( FReader->GetFileSize( Name ) );

		auto File = clMemRawFile::CreateFromManagedBuffer( DataPtr, FileSize );
		File->SetFileName( VirtualName );
		File->SetVirtualFileName( VirtualName );

		return File;
	}

	virtual bool FileExists( const std::string& VirtualName ) const override
	{ return FReader->FileExists( Arch_FixFileName( VirtualName ) ); }

	virtual std::string MapName( const std::string& VirtualName ) const override
	{ return VirtualName; }

private:
	clPtr<clArchiveReader> FReader;
};
