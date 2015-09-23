#include "Actors.h"
#include "Globals.h"
#include "Loader_OBJ.h"

void clSpaceShip::AttachToScene( const clPtr<clSceneNode>& Scene )
{
	if ( !m_Node )
	{
		auto Geometry = LoadOBJSceneNode( g_FS->CreateReader( "ship.obj" ) );

		sMaterial Material;
		Material.m_Ambient = vec4( 0.0f, 0.0f, 0.4f, 1.0f );
		Material.m_Diffuse = vec4( 0.0f, 0.0f, 0.6f, 1.0f );

		m_Node = make_intrusive<clMaterialNode>();
		m_Node->SetMaterial( Material );
		m_Node->Add( Geometry );
	}

	Scene->Add( m_Node );
}

void clSpaceShip::DetachFromScene( const clPtr<clSceneNode>& Scene )
{
	Scene->Remove( m_Node );
}

void clSpaceShip::Fire()
{
	if ( m_FireTime > 0.0f ) { return; }

	const float FireCooldown = 1.0f; // seconds

	m_FireTime = FireCooldown;

	g_Game->FireRocket( m_Pos, m_Vel * Math::RandomInRange( 1.1f, 1.5f ) + GetDirection() );
}

void clSpaceShip::Update( float dt )
{
	iActor::Update( dt );

//	printf("vv = %f\n", g_ScreenJoystick->GetAxisValue(0));

	if ( g_Game->IsKeyPressed( SDLK_LEFT ) )
	{
		m_Angle += dt;
	}

	if ( g_Game->IsKeyPressed( SDLK_RIGHT ) )
	{
		m_Angle -= dt;
	}

	bool Accel = g_Game->IsKeyPressed( SDLK_UP );
	bool Decel = g_Game->IsKeyPressed( SDLK_DOWN );
	m_Accel = vec3( 0.0f );

	if ( Accel )
	{
		m_Accel = GetDirection();
	}

	if ( Decel )
	{
		m_Accel += -GetDirection();
	}

	if ( g_Game->IsKeyPressed( SDLK_SPACE ) )
	{
		Fire();
	}

	m_Pos = g_Game->ClampToLevel( m_Pos );
	m_Vel *= 0.99f;
	const float MaxVel = 1.1f;

	if ( m_Vel.Length() > MaxVel ) { m_Vel = ( m_Vel / m_Vel.Length() ) * MaxVel; }

	m_FireTime -= dt;

	if ( m_FireTime < 0 ) { m_FireTime = 0.0f; }

	mat4 ScaleFix = mat4::GetScaleMatrix( vec3( 0.1f ) );
	mat4 RotFix = mat4::GetRotateMatrixAxis( 90.0f * Math::DTOR, vec3( 0, 0, 1 ) );

	mat4 Pos = mat4::GetTranslateMatrix( m_Pos );
	mat4 Rot = mat4::GetRotateMatrixAxis( m_Angle, vec3( 0, 0, 1 ) );

	if ( m_Node ) { m_Node->SetLocalTransform( ScaleFix * RotFix * Rot * Pos ); }
}

void clAsteroid::Update( float dt )
{
	iActor::Update( dt );

	m_Angle += dt;
	m_Pos = g_Game->ClampToLevel( m_Pos );

	mat4 ScaleFix = mat4::GetScaleMatrix( vec3( 0.002f ) );
	mat4 Pos = mat4::GetTranslateMatrix( m_Pos );
	mat4 Rot = mat4::GetRotateMatrixAxis( m_Angle, vec3( 1, 1, 1 ) );

	if ( m_Node ) { m_Node->SetLocalTransform( ScaleFix * Rot * Pos ); }
}

void clAsteroid::AttachToScene( const clPtr<clSceneNode>& Scene )
{
	if ( !m_Node )
	{
		auto Geometry = LoadOBJSceneNode( g_FS->CreateReader( "deimos.obj" ) );

		sMaterial Material;
		Material.m_Ambient = vec4( 0.5f, 0.5f, 0.0f, 1.0f );
		Material.m_Diffuse = vec4( 0.5f, 0.5f, 0.0f, 1.0f );

		m_Node = make_intrusive<clMaterialNode>();
		m_Node->SetMaterial( Material );
		m_Node->Add( Geometry );
	}

	Scene->Add( m_Node );
}

void clAsteroid::DetachFromScene( const clPtr<clSceneNode>& Scene )
{
	Scene->Remove( m_Node );
}

void clExplosion::Update( float dt )
{
	iActor::Update( dt );

	mat4 ScaleFix = mat4::GetScaleMatrix( vec3( 1.0f ) );
	mat4 Pos = mat4::GetTranslateMatrix( m_Pos );

	if ( m_Node )
	{
		const float SlowMotionCoef = 0.1f;

		m_Node->SetLocalTransform( ScaleFix * Pos );
		m_Node->UpdateParticles( SlowMotionCoef * dt );
	}

	if ( !m_Node->GetParticleSystem()->GetParticles().size() )
	{
		g_Game->Kill( this );
	}
}

void clExplosion::AttachToScene( const clPtr<clSceneNode>& Scene )
{
	if ( !m_Node )
	{
		m_Node = make_intrusive<clParticleSystemNode>();

		const vec4 Pal[] =
		{
			vec4( 0.2f, 0.30f, 0.8f, 1.0f ),
			vec4( 0.7f, 0.25f, 0.3f, 1.0f ),
			vec4( 0.1f, 0.80f, 0.2f, 1.0f )
		};

		vec4 Color = Pal[ Math::RandomInRange( 0, 3 ) ];

		auto Emitter = make_intrusive<clParticleEmitter_Explosion>();
		Emitter->FCenter = vec3( 0.0f );
		Emitter->FSizeMin = 0.02f;
		Emitter->FSizeMax = 0.05f;
		Emitter->FLifetimeMin = 0.1f;
		Emitter->FLifetimeMax = 1.0f;
		Emitter->FMaxParticles = 10000;
		Emitter->FEmissionRate = 300;
		Emitter->FRadialVelocityMin = 1.0f;
		Emitter->FRadialVelocityMax = 2.0f;
		Emitter->FColorMin = Color;
		Emitter->FColorMax = Color;
		Emitter->FAcceleration = 10.0f * m_ParticleAccel; //vec3( 0.0f, 0.0f, -3.0f );

		m_Node->AddEmitter( Emitter );
	}

	Scene->Add( m_Node );
}

void clExplosion::DetachFromScene( const clPtr<clSceneNode>& Scene )
{
	Scene->Remove( m_Node );
}

void clRocket::AttachToScene( const clPtr<clSceneNode>& Scene )
{
	if ( !m_Node )
	{
		auto VA = clGeomServ::CreateAxisAlignedBox( vec3( -0.02f ), vec3( +0.02f ) );
		auto Geometry = make_intrusive<clGeometryNode>();
		Geometry->SetVertexAttribs( VA );

		sMaterial Material;
		Material.m_Ambient = vec4( 0.8f, 0.0f, 0.0f, 1.0f );
		Material.m_Diffuse = vec4( 0.2f, 0.0f, 0.0f, 1.0f );

		m_Node = make_intrusive<clMaterialNode>();
		m_Node->SetMaterial( Material );
		m_Node->Add( Geometry );
	}

	Scene->Add( m_Node );

	Update( 0.0f );
}

void clRocket::DetachFromScene( const clPtr<clSceneNode>& Scene )
{
	Scene->Remove( m_Node );
}

void clRocket::Update( float dt )
{
	iActor::Update( dt );

	mat4 Pos = mat4::GetTranslateMatrix( m_Pos );

	if ( m_Node ) { m_Node->SetLocalTransform( Pos ); }

	if ( !g_Game->IsInsideLevel( m_Pos ) )
	{
		g_Game->Kill( this );
	}
}
