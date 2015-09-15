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

#include "FileSystem.h"
#include "Files.h"
#include "MountPoint.h"
#include "Archive.h"
#include "Blob.h"

#include <algorithm>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <direct.h>
#else  // any POSIX
#  include <dirent.h>
#  include <stdlib.h>
#endif

clPtr<iIStream> clFileSystem::CreateReader( const std::string& FileName ) const
{
	std::string Name = Arch_FixFileName( FileName );

	clPtr<iMountPoint> MountPoint = FindMountPoint( Name );

	if ( !MountPoint ) { return make_intrusive<clFileMapper>( make_intrusive<clNullRawFile>() ); }

	return make_intrusive<clFileMapper>( MountPoint->CreateReader( Name ) );
}

clPtr<iIStream> clFileSystem::CreateReaderFromString( const std::string& Str )
{
	return make_intrusive<clFileMapper>( clMemRawFile::CreateFromString( Str ) );
}

clPtr<iIStream> clFileSystem::CreateReaderFromMemory( const void* BufPtr, size_t BufSize, bool OwnsData )
{
	auto RawFile = make_intrusive<clMemRawFile>( reinterpret_cast<const uint8_t*>( BufPtr ), BufSize, OwnsData );

	return make_intrusive<clFileMapper>( RawFile );
}

clPtr<iIStream> clFileSystem::CreateReaderFromBlob( const clPtr<clBlob>& Blob )
{
	auto RawFile = make_intrusive<clManagedMemRawFile>();
	RawFile->SetBlob( Blob );

	return make_intrusive<clFileMapper>( RawFile );
}

clPtr<clMemFileWriter> clFileSystem::CreateMemWriter( const std::string& FileName, uint64_t InitialSize )
{
	auto Blob = make_intrusive<clBlob>();
	Blob->Reserve( static_cast<size_t>( InitialSize ) );

	auto Stream = make_intrusive<clMemFileWriter>( Blob );
	Stream->SetFileName( FileName );

	return Stream;
}

bool clFileSystem::FileExists( const std::string& Name ) const
{
	if ( Name.empty() || Name == "." ) { return false; }

	clPtr<iMountPoint> MP = FindMountPoint( Name );

	return MP ? MP->FileExists( Name ) : false;
}

std::string clFileSystem::VirtualNameToPhysical( const std::string& Path ) const
{
	if ( FS_IsFullPath( Path ) ) { return Path; }

	clPtr<iMountPoint> MP = FindMountPoint( Path );
	return ( !MP ) ? Path : MP->MapName( Path );
}

void clFileSystem::Mount( const std::string& PhysicalPath )
{
	clPtr<iMountPoint> MP;

	if ( Str::EndsWith( PhysicalPath, ".apk" ) || Str::EndsWith( PhysicalPath, ".zip" ) )
	{
		auto Reader = make_intrusive<clArchiveReader>();

		bool Result = Reader->OpenArchive( CreateReader( PhysicalPath ) );

		MP = make_intrusive<clArchiveMountPoint>( Reader );
	}
	else
	{
		MP = make_intrusive<clPhysicalMountPoint>( PhysicalPath );
	}

	MP->SetName( PhysicalPath );
	AddMountPoint( MP );
}

void clFileSystem::AddAliasMountPoint( const std::string& SrcPath, const std::string& AliasPrefix )
{
	clPtr<iMountPoint> MP = FindMountPointByName( SrcPath );

	if ( !MP ) { return; }

	auto AMP = make_intrusive<clAliasMountPoint>( MP );
	AMP->SetAlias( AliasPrefix );
	AddMountPoint( AMP );
}

clPtr<iMountPoint> clFileSystem::FindMountPointByName( const std::string& ThePath )
{
	for ( const auto& i : FMountPoints )
	{
		if ( i->GetName() == ThePath ) { return i; }
	}

	return nullptr;
}

void clFileSystem::AddMountPoint( const clPtr<iMountPoint>& MP )
{
	if ( !MP ) { return; }

	if ( std::find( FMountPoints.begin(), FMountPoints.end(), MP ) == FMountPoints.end() ) { FMountPoints.push_back( MP ); }
}

clPtr<iMountPoint> clFileSystem::FindMountPoint( const std::string& FileName ) const
{
	if ( FMountPoints.empty() ) { return nullptr; }

	if ( ( *FMountPoints.begin() )->FileExists( FileName ) )
	{
		return ( *FMountPoints.begin() );
	}

	// reverse order
	for ( auto i = FMountPoints.rbegin(); i != FMountPoints.rend(); ++i )
	{
		if ( ( *i )->FileExists( FileName ) ) { return ( *i ); }
	}

	return *( FMountPoints.begin() );
}

clPtr<clBlob> LoadFileAsBlob( const clPtr<clFileSystem>& FileSystem, const std::string& Name )
{
	auto Input = FileSystem->CreateReader( Name );
	auto Res = make_intrusive<clBlob>();
	Res->AppendBytes( Input->MapStream(), static_cast<size_t>( Input->GetSize() ) );
	return Res;
}
