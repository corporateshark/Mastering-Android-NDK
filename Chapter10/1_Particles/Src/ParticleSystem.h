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

#include "Platform.h"
#include "VecMath.h"
#include "Geometry.h"
#include "Material.h"

class clMaterial;

struct sParticle
{
	sParticle(): FPosition(),
		FVelocity(),
		FAcceleration(),
		FLifeTime( 0.0f ),
		FTTL( 0.0f ),
		FRGBA( 1.0f, 1.0f, 1.0f, 1.0f ),
		FSize( 0.5f ) {};
	//
	LVector3    FPosition;             // current position
	LVector3    FVelocity;             // current velocity
	LVector3    FAcceleration;         // current accelleration
	float       FLifeTime;             // total life time
	float       FTTL;                  // time to live left
	LVector4    FRGBA;                 // overlay color
	float       FSize;                 // particle size
};

class clParticleSystem: public iIntrusiveCounter
{
public:
	clParticleSystem();

	virtual const sMaterial& GetDefaultMaterial() const { return FMaterial; }
	virtual sMaterial& GetDefaultMaterial() { return FMaterial; }
	virtual void      UpdateParticles( float DeltaSeconds );
	virtual void      AddParticle( const sParticle& Particle );
	virtual std::vector<sParticle>& GetParticles()
	{
		return FParticles;
	};
	virtual clPtr<clVertexAttribs> GetVertices() const
	{
		return FVertices;
	};
	void SetMaxParticles( int MaxParticles );

private:
	std::vector<sParticle> FParticles;
	int                    FCurrentMaxParticles;
	clPtr<clVertexAttribs> FVertices;
	sMaterial              FMaterial;
};
