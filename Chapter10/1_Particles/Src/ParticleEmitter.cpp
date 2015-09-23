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

#include "ParticleEmitter.h"
#include "ParticleSystem.h"

void clParticleEmitter_Box::EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const
{
	FAccumulatedTime += DeltaTime;

	while (
	   FAccumulatedTime > 1.0f / FEmissionRate &&
	   PS->GetParticles().size() < FMaxParticles )
	{
		FAccumulatedTime -= 1.0f / FEmissionRate;
		sParticle P;
		P.FPosition = Math::RandomVector3InRange( FPosMin, FPosMax );
		P.FVelocity = Math::RandomVector3InRange( FVelMin, FVelMax );
		P.FAcceleration = LVector3( 0.0f );
		P.FTTL = Math::RandomInRange( FLifetimeMin, FLifetimeMax );
		P.FLifeTime = P.FTTL;
		P.FRGBA = Math::RandomVector4InRange( FColorMin, FColorMax );
		P.FRGBA.w = 1.0f;
		P.FSize = Math::RandomInRange( FSizeMin, FSizeMax );
		PS->AddParticle( P );
	}
}

void clParticleEmitter_Sphere::EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const
{
	FAccumulatedTime += DeltaTime;

	while (
	   FAccumulatedTime > 1.0f / FEmissionRate &&
	   PS->GetParticles().size() < FMaxParticles )
	{
		FAccumulatedTime -= 1.0f / FEmissionRate;
		sParticle P;

		float Theta = Math::RandomInRange( 0.0f, Math::TWOPI );
		float Phi   = Math::RandomInRange( 0.0f, Math::TWOPI );
		float R     = FRadius;

		float SinTheta = sin( Theta );

		float x = R * SinTheta * cos( Phi );
		float y = R * SinTheta * sin( Phi );
		float z = R * cos( Theta );

		P.FPosition = FCenter + vec3( x, y, z );
		P.FVelocity = vec3( x, y, z ).GetNormalized() * Math::RandomInRange( FRadialVelocityMin, FRadialVelocityMax );
		P.FAcceleration = vec3( 0.0f );
		P.FTTL = Math::RandomInRange( FLifetimeMin, FLifetimeMax );
		P.FLifeTime = P.FTTL;
		P.FRGBA = Math::RandomVector4InRange( FColorMin, FColorMax );
		P.FRGBA.w = 1.0f;
		P.FSize = Math::RandomInRange( FSizeMin, FSizeMax );
		PS->AddParticle( P );
	}
}

void clParticleEmitter_Explosion::EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const
{
	// emit secondary particles
	auto& Particles = PS->GetParticles();

	size_t OriginalSize = Particles.size();

	for ( size_t i = 0; i != OriginalSize; i++ )
	{
		if ( Particles[i].FRGBA.w > 0.99f && Particles.size() < FMaxParticles )
		{
			// create a secondary particle
			sParticle P;

			P.FPosition = Particles[i].FPosition;
			P.FVelocity = Particles[i].FVelocity * Math::RandomVector3InRange( vec3( 0.1f ), vec3( 1.0f ) );
			P.FAcceleration = FAcceleration;
			P.FTTL = Particles[i].FTTL * 0.5f;
			P.FLifeTime = P.FTTL;
			P.FRGBA = Particles[i].FRGBA * Math::RandomVector4InRange( vec4( 0.5f ), vec4( 0.9f ) );
			P.FRGBA.w = 0.95f;
			P.FSize = Particles[i].FSize * Math::RandomInRange( 0.1f, 0.9f );

			PS->AddParticle( P );
		}
	}

	if ( FEmitted ) { return; }

	FEmitted = true;

	for ( size_t i = 0; i != FEmissionRate; i++ )
	{
		sParticle P;

		float Theta = Math::RandomInRange( 0.0f, Math::TWOPI );
		float Phi   = Math::RandomInRange( 0.0f, Math::TWOPI );

		float SinTheta = sin( Theta );

		float x = SinTheta * cos( Phi );
		float y = SinTheta * sin( Phi );
		float z = cos( Theta );

		P.FPosition = FCenter;
		P.FVelocity = vec3( x, y, z ).GetNormalized() * Math::RandomInRange( FRadialVelocityMin, FRadialVelocityMax );
		P.FAcceleration = FAcceleration;
		P.FTTL = Math::RandomInRange( FLifetimeMin, FLifetimeMax );
		P.FLifeTime = P.FTTL;
		P.FRGBA = Math::RandomVector4InRange( FColorMin, FColorMax );
		P.FRGBA.w = 1.0f;
		P.FSize = Math::RandomInRange( FSizeMin, FSizeMax );
		PS->AddParticle( P );
	}
}
