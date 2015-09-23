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

#include <algorithm>

#include "ParticleNode.h"

clParticleSystemNode::clParticleSystemNode()
{
	// create the system
	m_Particles = make_intrusive<clParticleSystem>();

	size_t MaxParticles = 20000;

	for ( const auto& i : m_Emitters )
	{
		if ( i->FMaxParticles > MaxParticles ) { MaxParticles = i->FMaxParticles; }
	}

	m_Particles->SetMaxParticles( static_cast<int>( MaxParticles ) );

	// create subnode
	m_ParticlesNode = make_intrusive<clGeometryNode>();
	m_ParticlesNode->SetVertexAttribs( m_Particles->GetVertices() );

	this->Add( m_ParticlesNode );

	// set material
	this->SetMaterial( m_Particles->GetDefaultMaterial() );

	UpdateParticles( 0.0f );
}

void clParticleSystemNode::AddEmitter( const clPtr<iParticleEmitter>& E )
{
	m_Emitters.push_back( E );
};

void clParticleSystemNode::UpdateParticles( float DeltaSeconds )
{
	for ( const auto& i : m_Emitters )
	{
		i->EmitParticles( m_Particles, DeltaSeconds );
	}

	m_Particles->UpdateParticles( DeltaSeconds );
//	m_ParticlesNode->SetVertexAttribs( m_Particles->GetVertices() );
	m_ParticlesNode->GetVA()->CommitChanges();
}

void clParticleSystemNode::RemoveEmitter( const clPtr<iParticleEmitter>& E )
{
	m_Emitters.erase( std::remove( m_Emitters.begin(), m_Emitters.end(), E ), m_Emitters.end() );
}
