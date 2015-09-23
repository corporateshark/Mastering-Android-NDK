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

#include "IntrusivePtr.h"
#include "VecMath.h"

class clParticleSystem;

class iParticleEmitter: public iIntrusiveCounter
{
public:
	iParticleEmitter()
		: FColorMin( 0 )
		, FColorMax( 1 )
		, FSizeMin( 0.5f )
		, FSizeMax( 1.0f )
		, FVelMin( 0 )
		, FVelMax( 0 )
		, FMaxParticles( 1000 )
		  // particles per second
		, FEmissionRate( 100.0f )
		, FLifetimeMin( 1.0f )
		, FLifetimeMax( 60.0f )
		  //
		, FInvEmissionRate( 1.0f / FEmissionRate )
		, FAccumulatedTime( 0.0f )
	{}

	virtual void EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const = 0;

public:
	LVector4 FColorMin;
	LVector4 FColorMax;
	float    FSizeMin;
	float    FSizeMax;
	LVector3 FVelMin;
	LVector3 FVelMax;

	size_t   FMaxParticles;

	// particles per second
	float FEmissionRate;

	// seconds
	float FLifetimeMin;
	float FLifetimeMax;

protected:
	float FInvEmissionRate;
	mutable float FAccumulatedTime;
};

class clParticleEmitter_Box: public iParticleEmitter
{
public:
	clParticleEmitter_Box()
		: FPosMin( 0 )
		, FPosMax( 1 )
	{};

	//
	// iParticleEmitter interface
	//
	virtual void EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const override;

public:
	LVector3 FPosMin;
	LVector3 FPosMax;
};

class clParticleEmitter_Sphere: public iParticleEmitter
{
public:
	clParticleEmitter_Sphere()
		: FCenter( 0.0f )
		, FRadius( 1.0f )
		, FRadialVelocityMin( 0.1f )
		, FRadialVelocityMax( 1.0f )
	{}

	//
	// iParticleEmitter interface
	//
	virtual void EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const override;

public:
	vec3 FCenter;
	float FRadius;
	float FRadialVelocityMin;
	float FRadialVelocityMax;
};

class clParticleEmitter_Explosion: public iParticleEmitter
{
public:
	clParticleEmitter_Explosion()
		: FEmitted( false )
		, FCenter( 0.0f )
		, FRadialVelocityMin( 0.1f )
		, FRadialVelocityMax( 1.0f )
		, FAcceleration( 0.0f )
	{}

	virtual void EmitParticles( const clPtr<clParticleSystem>& PS, float DeltaTime ) const override;

public:
	mutable bool FEmitted;
	vec3 FCenter;
	float FRadialVelocityMin;
	float FRadialVelocityMax;
	vec3 FAcceleration;
};
