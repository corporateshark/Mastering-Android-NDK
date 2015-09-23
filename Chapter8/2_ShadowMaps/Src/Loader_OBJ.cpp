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

#define _CRT_SECURE_NO_WARNINGS

#include "Loader_OBJ.h"

#include "Nodes.h"

struct sOBJVertex
{
	explicit sOBJVertex( const LVector3& Pos ): FPos( Pos ) {};
	LVector3 FPos;
};

struct sOBJNormal
{
	explicit sOBJNormal( const LVector3& Normal ): FNormal( Normal ) {};
	LVector3 FNormal;
};

struct sOBJTexCoord
{
	explicit sOBJTexCoord( const LVector2& TexCoord ): FTexCoord( TexCoord ) {};
	LVector2 FTexCoord;
};

struct sOBJFace
{
	sOBJFace()
		: V1( 0 ), V2( 0 ), V3( 0 ), Vt1( 0 ), Vt2( 0 ), Vt3( 0 ), Vn1( 0 ), Vn2( 0 ), Vn3( 0 )
	{}
	int V1, V2, V3;
	int Vt1, Vt2, Vt3;
	int Vn1, Vn2, Vn3;
};

struct sOBJMesh: public iIntrusiveCounter
{
	std::string m_UseMtl;
	std::vector<sOBJFace> m_Faces;

	inline void EmitFace( const sOBJFace& Face )
	{
		m_Faces.push_back( Face );
	}
};

struct sOBJFile
{
	std::vector<sOBJVertex>   FVertices;
	std::vector<sOBJNormal>   FNormals;
	std::vector<sOBJTexCoord> FTexCoords;

	inline void EmitVertex( const LVector3& Pos )
	{
		FVertices.push_back( sOBJVertex( Pos ) );
	}
	inline void EmitNormal( const LVector3& Normal )
	{
		FNormals.push_back( sOBJNormal( Normal ) );
	}
	inline void EmitTexCoord( const LVector2& TexCoord )
	{
		FTexCoords.push_back( sOBJTexCoord( TexCoord ) );
	}
};

std::string ReadLine( const clPtr<iIStream>& IStream )
{
	const size_t MAX_LINE_WIDTH = 2 * 65535;

	char LineBuffer[MAX_LINE_WIDTH + 1];

	char* Out = LineBuffer;
	char* End = LineBuffer + MAX_LINE_WIDTH;

	while ( !IStream->Eof() && Out < End )
	{
		char Ch;
		IStream->Read( &Ch, 1 );

		if ( Ch == 13 ) { continue; }   // kill char

		if ( Ch == 10 ) { break; }

		*Out++ = Ch;
	}

	( *Out ) = 0;

	return std::string( LineBuffer );
}

bool PopulateMesh( const clPtr<iIStream>& IStream, sOBJFile* File, sOBJMesh* Mesh )
{
	char Prefix[7];

	float X, Y, Z;
	int A1, A2, A3, B1, B2, B3, C1, C2, C3, D1, D2, D3;

	while ( !IStream->Eof() )
	{
		Luint64 Pos = IStream->GetPos();
		std::string Line = ReadLine( IStream );

		int NumRead = sscanf( Line.c_str(), "%6s %f %f %f", Prefix, &X, &Y, &Z );

		if ( NumRead < 1 ) { continue; }

		switch ( Prefix[0] )
		{
			case '#':
				continue;

			case 'v':
			{
				switch ( Prefix[1] )
				{
					case 0:
						if ( NumRead != 4 ) { continue; }

						File->EmitVertex( LVector3( X, Z, Y ) );
						break;

					case 't':
						if ( NumRead < 3 ) { continue; }

						File->EmitTexCoord( LVector2( X, 1.0f - Y ) );
						break;

					case 'n':
						if ( NumRead != 4 ) { continue; }

						File->EmitNormal( LVector3( X, Z, Y ) );
						break;
				}
			}
			break;

			case 'f':
			{
				if ( sscanf( Line.c_str(), "%2s %d/%d %d/%d %d/%d %d/%d", Prefix, &A1, &A2, &B1, &B2, &C1, &C2, &D1, &D2 ) == 9 )
				{
					A3 = B3 = C3 = 0;
					sOBJFace Face;
					Face.Vt1 = A2;
					Face.Vt2 = B2;
					Face.Vt3 = D2;
					Face.V1 = A1;
					Face.V2 = B1;
					Face.V3 = D1;
					Mesh->EmitFace( Face );

					Face.Vt1 = B2;
					Face.Vt2 = C2;
					Face.Vt3 = D2;
					Face.V1 = B1;
					Face.V2 = C1;
					Face.V3 = D1;
					Mesh->EmitFace( Face );
				}
				else if ( sscanf( Line.c_str(), "%2s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", Prefix, &A1, &A2, &A3, &B1, &B2, &B3, &C1, &C2, &C3, &D1, &D2, &D3 ) == 13 )
				{
					sOBJFace Face;
					Face.Vt1 = A2;
					Face.Vt2 = B2;
					Face.Vt3 = D2;
					Face.Vn1 = A3;
					Face.Vn2 = B3;
					Face.Vn3 = D3;
					Face.V1 = A1;
					Face.V2 = B1;
					Face.V3 = D1;
					Mesh->EmitFace( Face );

					Face.Vt1 = B2;
					Face.Vt2 = C2;
					Face.Vt3 = D2;
					Face.Vn1 = B3;
					Face.Vn2 = C3;
					Face.Vn3 = D3;
					Face.V1 = B1;
					Face.V2 = C1;
					Face.V3 = D1;
					Mesh->EmitFace( Face );
				}
				else if ( sscanf( Line.c_str(), "%2s %d/%d/%d %d/%d/%d %d/%d/%d", Prefix, &A1, &A2, &A3, &B1, &B2, &B3, &C1, &C2, &C3 ) == 10 )
				{
					sOBJFace Face;
					Face.Vt1 = A2;
					Face.Vt2 = B2;
					Face.Vt3 = C2;
					Face.Vn1 = A3;
					Face.Vn2 = B3;
					Face.Vn3 = C3;
					Face.V1 = A1;
					Face.V2 = B1;
					Face.V3 = C1;
					Mesh->EmitFace( Face );
				}
				else if ( sscanf( Line.c_str(), "%2s %d//%d %d//%d %d//%d", Prefix, &A1, &A3, &B1, &B3, &C1, &C3 ) == 7 )
				{
					A2 = B2 = C2 = 0;
					sOBJFace Face;
					Face.V1 = A1;
					Face.V2 = B1;
					Face.V3 = C1;
					Face.Vn1 = A3;
					Face.Vn2 = B3;
					Face.Vn3 = C3;
					Mesh->EmitFace( Face );
				}
				else if ( sscanf( Line.c_str(), "%2s %d/%d %d/%d %d/%d", Prefix, &A1, &A2, &B1, &B2, &C1, &C2 ) == 7 )
				{
					A3 = B3 = C3 = 0;
					sOBJFace Face;
					Face.Vt1 = A2;
					Face.Vt2 = B2;
					Face.Vt3 = C2;
					Face.V1 = A1;
					Face.V2 = B1;
					Face.V3 = C1;
					Mesh->EmitFace( Face );
				}

			}
			break;
		}
	}

	return false;
}

clPtr<clVertexAttribs> CreateVertexAttribs( const sOBJFile& File, const sOBJMesh& Mesh )
{
	clPtr<clVertexAttribs> VA = make_intrusive<clVertexAttribs>();

	bool HasNormals = File.FNormals.size() > 0;

	VA->Restart( Mesh.m_Faces.size() * 3 );

	for ( size_t i = 0; i != Mesh.m_Faces.size(); i++ )
	{
		const sOBJFace& Face = Mesh.m_Faces[i];

		if ( HasNormals ) { VA->SetNormalV( File.FNormals[ Face.Vn1 - 1 ].FNormal ); }

		if ( Face.Vt1 ) { VA->SetTexCoordV( File.FTexCoords[ Face.Vt1 - 1 ].FTexCoord ); }

		VA->EmitVertexV( File.FVertices[ Face.V1 - 1 ].FPos );

		if ( HasNormals ) { VA->SetNormalV( File.FNormals[ Face.Vn2 - 1 ].FNormal ); }

		if ( Face.Vt2 ) { VA->SetTexCoordV( File.FTexCoords[ Face.Vt2 - 1 ].FTexCoord ); }

		VA->EmitVertexV( File.FVertices[ Face.V2 - 1 ].FPos );

		if ( HasNormals ) { VA->SetNormalV( File.FNormals[ Face.Vn3 - 1 ].FNormal ); }

		if ( Face.Vt3 ) { VA->SetTexCoordV( File.FTexCoords[ Face.Vt3 - 1 ].FTexCoord ); }

		VA->EmitVertexV( File.FVertices[ Face.V3 - 1 ].FPos );
	}

	return VA;
}

clPtr<clSceneNode> LoadOBJSceneNode( const clPtr<iIStream>& IStream )
{
	clPtr<clSceneNode> Root = make_intrusive<clSceneNode>();

	bool HasMore = true;

	sOBJFile File;

	std::vector< clPtr<sOBJMesh> > Meshes;

	// create geometry data
	while ( HasMore )
	{
		Meshes.push_back( new sOBJMesh() );

		HasMore = PopulateMesh( IStream, &File, Meshes.back().GetInternalPtr() );
	}

	// create scene nodes
	for ( size_t i = 0; i != Meshes.size(); i++ )
	{
		clPtr<sOBJMesh> Mesh = Meshes[i];

		clPtr<clVertexAttribs> VA = CreateVertexAttribs( File, *Mesh.GetInternalPtr( ) );

		auto Node = make_intrusive<clGeometryNode>();
		Node->SetVertexAttribs( VA );
		Root->Add( Node );
	}

	return Root;
}
