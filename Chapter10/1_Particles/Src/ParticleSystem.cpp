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

#include "ParticleSystem.h"
#include "Geometry.h"
#include "LGL/GLClasses.h"

clParticleSystem::clParticleSystem()
	: FCurrentMaxParticles( 100 )
{
	FVertices = make_intrusive<clVertexAttribs>( 6 * FCurrentMaxParticles );
	FMaterial.m_MaterialClass = "Particle";
}

void clParticleSystem::SetMaxParticles( int MaxParticles )
{
	FCurrentMaxParticles = MaxParticles;

	FParticles.reserve( FCurrentMaxParticles );
	FVertices = make_intrusive<clVertexAttribs>( 6 * MaxParticles );

	// prefetch tex coordinates for our GPU billboarder
	vec2* Vec = FVertices->FTexCoords.data();

	for ( int i = 0; i != MaxParticles; ++i )
	{
		int IdxI = i * 6;

		Vec[IdxI + 0] = vec2( 0.0f, 0.0f );
		Vec[IdxI + 1] = vec2( 1.0f, 0.0f );
		Vec[IdxI + 2] = vec2( 1.0f, 1.0f );

		Vec[IdxI + 3] = vec2( 0.0f, 0.0f );
		Vec[IdxI + 4] = vec2( 1.0f, 1.0f );
		Vec[IdxI + 5] = vec2( 0.0f, 1.0f );
	}
}

void clParticleSystem::AddParticle( const sParticle& Particle )
{
	FParticles.push_back( Particle );

	if ( FCurrentMaxParticles < static_cast<int>( FParticles.size() ) )
	{
		SetMaxParticles( int( FCurrentMaxParticles * 1.2f ) );
	}
}

inline float GetParticleBrightness( float NormalizedTime )
{
	const float Cutoff_Lo = 0.1f;
	const float Cutoff_Hi = 0.9f;

	if ( NormalizedTime < Cutoff_Lo )
	{
		return NormalizedTime / Cutoff_Lo;
	}

	if ( NormalizedTime > Cutoff_Hi )
	{
		return 1.0f - ( ( NormalizedTime - Cutoff_Hi ) / ( 1.0f - Cutoff_Hi ) );
	}

	return 1.0f;
}

void clParticleSystem::UpdateParticles( float DeltaSeconds )
{
	vec3* Vec  = FVertices->FVertices.data();
	vec3* Norm = FVertices->FNormals.data();
	vec4* RGB  = FVertices->FColors.data();

	size_t NumParticles = FParticles.size();

	for ( size_t i = 0; i != NumParticles; ++i )
	{
		sParticle& P = FParticles[i];

		P.FTTL -= DeltaSeconds;

		if ( P.FTTL < 0 )
		{
			P = FParticles.back();
			FParticles.pop_back();
			NumParticles--;
			i--;

			continue;
		}

		P.FVelocity += P.FAcceleration * DeltaSeconds;
		P.FPosition += P.FVelocity * DeltaSeconds;

		size_t IdxI = i * 6;

		// save lifetimes and size
		LVector3 TTL = LVector3( P.FTTL, P.FLifeTime, P.FSize );

		float NormalizedTime = ( P.FLifeTime - P.FTTL ) / P.FLifeTime;

		vec4 Color = P.FRGBA * GetParticleBrightness( NormalizedTime );

		// save positions, TTL and RGB color for particles
		for ( int j = 0 ; j < 6 ; j++ )
		{
			Vec [IdxI + j] = P.FPosition;
			Norm[IdxI + j] = TTL;
			RGB [IdxI + j] = Color;
		}
	}

	FVertices->SetActiveVertexCount( 6 * static_cast<int>( FParticles.size() ) );
}
