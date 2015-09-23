/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
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

#include <random>
#include "VecMath.h"

inline vec2 ClampVec2( const vec2& V, float MaxValue )
{
	float L = V.Length();

	return ( L > MaxValue ) ? V.GetNormalized() * MaxValue : V;
}

std::random_device rd;
std::mt19937 gen( rd() );
std::uniform_real_distribution<> dis( 0.0, 1.0 );

float RandomFloat()
{
	return static_cast<float>( dis( gen ) );
}

vec2 RandomVec2Range( const vec2& Min, const vec2& Max )
{
	return Min + vec2( RandomFloat() * ( Max - Min ).x,
	                   RandomFloat() * ( Max - Min ).y );
}

class clBoid;
class clSwarm;

class iActor: public iIntrusiveCounter
{
public:
	virtual void Update( float dt ) = 0;
};

class iBehaviour: public iIntrusiveCounter
{
public:
	virtual vec2 GetControl( float dt, clBoid* Boid ) = 0;
};

class clBoid: public iActor
{
public:
	clBoid()
		: m_Pos()
		, m_Vel()
		, m_Angle( 0.0f )
		, m_MaxVel( 1.0f )
		, m_Behaviour()
		, m_Swarm( nullptr )
	{}

	virtual void Update( float dt ) override;

public:
	vec2 m_Pos;
	vec2 m_Vel;
	float m_Angle;

	float m_MaxVel;

	clPtr<iBehaviour> m_Behaviour;
	clSwarm* m_Swarm;
};

void clBoid::Update( float dt )
{
	if ( m_Behaviour )
	{
		vec2 Force = m_Behaviour->GetControl( dt, this );
		const float Mass = 1.0f;
		vec2 Accel = Force / Mass;
		m_Vel += Accel * dt;
	}

	m_Vel = ClampVec2( m_Vel, m_MaxVel );
	m_Pos += m_Vel * dt;

	if ( m_Vel.SqrLength() > 0.0f )
	{
		m_Angle = atan2( m_Vel.y, m_Vel.x );
	}
}

class clWanderBehaviour: public iBehaviour
{
public:
	virtual vec2 GetControl( float dt, clBoid* Boid ) override;
};

vec2 clWanderBehaviour::GetControl( float dt, clBoid* Boid )
{
	return vec2( RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f );
}

class clSwarm: public iIntrusiveCounter
{
public:
	clSwarm() {}

	void GenerateRandom( size_t N )
	{
		m_Boids.reserve( N );

		for ( size_t i = 0; i != N; i++ )
		{
			m_Boids.emplace_back( make_intrusive<clBoid>() );
			m_Boids.back()->m_Behaviour = make_intrusive<clWanderBehaviour>();
			m_Boids.back()->m_Swarm = this;
			m_Boids.back()->m_Pos = RandomVec2Range( vec2( -1, -1 ), vec2( 1, 1 ) );
		}
	}

	void Update( float dt )
	{
		for ( auto& i : m_Boids )
		{
			i->Update( dt );
		}
	}

	vec2 CalculateSeparation( clBoid* B, float SafeDistance )
	{
		vec2 Control;

		for ( auto& i : m_Boids )
		{
			if ( i.GetInternalPtr() != B )
			{
				auto Delta = i->m_Pos - B->m_Pos;

				if ( Delta.Length() < SafeDistance )
				{
					Control += Delta;
				}
			}
		}

		return Control;
	}

	vec2 CalculateAverageNeighboursPosition( clBoid* B )
	{
		int N = ( int )m_Boids.size();

		if ( N > 1 )
		{
			vec2 Avg( 0, 0 );

			for ( auto& i : m_Boids )
			{
				if ( i.GetInternalPtr() != B )
				{
					Avg += i->m_Pos;
				}
			}

			Avg *= 1.0f / ( float )( N - 1 );

			return Avg;
		}

		return B->m_Pos;
	}

	vec2 CalculateAverageNeighboursVelocity( clBoid* B )
	{
		int N = ( int )m_Boids.size();

		if ( N > 1 )
		{
			vec2 Avg( 0, 0 );

			for ( auto& i : m_Boids )
			{
				if ( i.GetInternalPtr() != B )
				{
					Avg += i->m_Vel;
				}
			}

			Avg *= 1.0f / ( float )( N - 1 );

			return Avg;
		}

		return B->m_Vel;
	}

	void SetSingleBehaviour( const clPtr<iBehaviour>& B )
	{
		for ( auto& i : m_Boids )
		{
			i->m_Behaviour = B;
		}
	}

	std::vector< clPtr<clBoid> > m_Boids;
};

class clGoToBehaviour: public iBehaviour
{
public:
	clGoToBehaviour()
		: m_Target()
		, m_TargetRadius( 0.1f )
		, m_VelGain( 0.05f )
		, m_PosGain( 1.0f )
	{}

	virtual vec2 GetControl( float dt, clBoid* Boid ) override
	{
		auto Delta = m_Target - Boid->m_Pos;

		if ( Delta.Length() < m_TargetRadius )
		{
			return vec2(); // -m_VelGain * Boid->m_Vel / dt;
		}

		return Delta * m_PosGain;
	}

	vec2 m_Target;
	float m_TargetRadius;
	float m_VelGain;
	float m_PosGain;
};

class clFlockingBehaviour : public iBehaviour
{
public:
	clFlockingBehaviour()
		: m_AlignmentGain( 0.1f )
		, m_AvoidanceGain( 2.0f )
		, m_CohesionGain( 0.1f )
		, m_SafeDistance( 0.5f )
		, m_MaxValue( 1.0f )
	{}

	float m_SafeDistance;
	float m_AvoidanceGain;
	float m_AlignmentGain;
	float m_CohesionGain;

	float m_MaxValue;

	virtual vec2 GetControl( float dt, clBoid* Boid ) override
	{
		auto Swarm = Boid->m_Swarm;
		// 1. Separation/Avoidance
		vec2 Sep = m_AvoidanceGain * Swarm->CalculateSeparation( Boid, m_SafeDistance );

		// 2. Alignment
		auto AvgPos = Swarm->CalculateAverageNeighboursPosition( Boid );
		vec2 Alignment = m_AlignmentGain * ( AvgPos - Boid->m_Pos );

		// 3. Cohesion - steer to average position of neighbours
		auto AvgVel = Swarm->CalculateAverageNeighboursVelocity( Boid );
		vec2 Cohesion = m_CohesionGain * ( AvgVel - Boid->m_Vel );

		return ClampVec2( Sep + Alignment + Cohesion, m_MaxValue );
	}
};

class clMixedBehaviour : public iBehaviour
{
public:
	void AddBehaviour( float Weight, const clPtr<iBehaviour>& B )
	{
		m_Weights.push_back( Weight );
		m_Behaviours.push_back( B );
	}

	virtual vec2 GetControl( float dt, clBoid* Boid ) override
	{
		vec2 Control;

		for ( size_t i = 0; i < m_Behaviours.size(); i++ )
		{
			Control += m_Weights[i] * m_Behaviours[i]->GetControl( dt, Boid );
		}

		return Control;
	}

	std::vector< clPtr<iBehaviour> > m_Behaviours;
	std::vector<float> m_Weights;
};
