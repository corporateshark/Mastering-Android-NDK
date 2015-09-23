#pragma once

#include "IntrusivePtr.h"
#include "VecMath.h"
#include "Nodes.h"
#include "ParticleNode.h"

class iActor: public iIntrusiveCounter
{
public:
	iActor()
		: m_Pos( 0 )
		, m_Vel( 0 )
		, m_Accel( 0 )
	{}

	virtual void AttachToScene( const clPtr<clSceneNode>& Scene ) = 0;
	virtual void DetachFromScene( const clPtr<clSceneNode>& Scene ) = 0;

	virtual void Update( float dt )
	{
		m_Vel += m_Accel * dt;
		m_Pos += m_Vel * dt;
	}

	virtual float GetRadius() const
	{
		return 0.1f;
	}

public:
	vec3 m_Pos;
	vec3 m_Vel;
	vec3 m_Accel;
};

class clAsteroid: public iActor
{
public:
	clAsteroid()
		: m_Angle( Math::RandomInRange( 0.0f, 1.0f ) )
	{}

	virtual void AttachToScene( const clPtr<clSceneNode>& Scene ) override;
	virtual void DetachFromScene( const clPtr<clSceneNode>& Scene ) override;

	virtual void Update( float dt ) override;

private:
	clPtr<clMaterialNode> m_Node;
	float m_Angle;
};

class clExplosion: public iActor
{
public:
	clExplosion()
		: m_ParticleAccel( 0 )
	{}
	virtual void AttachToScene( const clPtr<clSceneNode>& Scene ) override;
	virtual void DetachFromScene( const clPtr<clSceneNode>& Scene ) override;

	virtual void Update( float dt ) override;

public:
	vec3 m_ParticleAccel;

private:
	clPtr<clParticleSystemNode> m_Node;
};

class clRocket: public iActor
{
public:
	clRocket()
		: m_Node()
	{}

	virtual void AttachToScene( const clPtr<clSceneNode>& Scene ) override;
	virtual void DetachFromScene( const clPtr<clSceneNode>& Scene ) override;

	virtual void Update( float dt ) override;

private:
	clPtr<clMaterialNode> m_Node;
};

class clSpaceShip: public iActor
{
public:
	clSpaceShip()
		: m_Node()
		, m_Angle( 0.0f )
		, m_FireTime( 0.0f )
	{}

	virtual void AttachToScene( const clPtr<clSceneNode>& Scene ) override;
	virtual void DetachFromScene( const clPtr<clSceneNode>& Scene ) override;

	virtual void Update( float dt ) override;

	//
	// clSpaceShip
	//

	virtual void Fire();
	virtual vec3 GetDirection() const
	{
		return vec3( cos( m_Angle ), sin( m_Angle ), 0.0f );
	}

private:
	clPtr<clMaterialNode> m_Node;
	float m_Angle;
	float m_FireTime;
};
