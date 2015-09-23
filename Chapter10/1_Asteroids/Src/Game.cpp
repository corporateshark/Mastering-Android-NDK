#include "Game.h"
#include "Globals.h"
#include "Actors.h"
#include "MOD.h"

clGameManager::clGameManager()
	: m_Keys()
	, m_LevelMin( -4.1f, -2.2f, 0.0f )
	, m_LevelMax( +4.1f, +2.2f, 0.0f )
{
	m_Scene = make_intrusive<clSceneNode>();

	m_SpaceShip = make_intrusive<clSpaceShip>();

	m_SpaceShip->AttachToScene( m_Scene );

	{
		auto Light = make_intrusive<iLight>();
		Light->m_Diffuse = vec4( 0.5f, 0.5f, 0.5f, 1.0f );
		Light->m_Position = vec3( 7, 7, 7 );
		Light->m_Direction = vec3( -1, -1, -1 ).GetNormalized();
		Light->m_SpotOuterAngle = 23;
		auto LightNode = make_intrusive<clLightNode>();
		LightNode->SetLight( Light );
		m_Scene->Add( LightNode );
	}
	{
		auto Light = make_intrusive<iLight>();
		Light->m_Diffuse = vec4( 0.5f, 0.5f, 0.5f, 1.0f );
		Light->m_Position = vec3( 7, -7, 7 );
		Light->m_Direction = vec3( -1, 1, -1 ).GetNormalized();
		Light->m_SpotOuterAngle = 23;
		auto LightNode = make_intrusive<clLightNode>();
		LightNode->SetLight( Light );
		m_Scene->Add( LightNode );
	}


	// plane
	{
		auto PlaneMaterialNode = make_intrusive<clMaterialNode>();
		sMaterial Material;
		Material.m_Ambient = vec4( 0.0f, 0.2f, 0.0f, 1.0f );
		Material.m_Diffuse = vec4( 0.0f, 0.8f, 0.0f, 1.0f );
		PlaneMaterialNode->SetMaterial( Material );

		auto VA = clGeomServ::CreateAxisAlignedBox(
		             vec3( m_LevelMin.x, m_LevelMin.y, -0.20f ),
		             vec3( m_LevelMax.x, m_LevelMax.y, -0.15f ) );
		auto Geometry = make_intrusive<clGeometryNode>();
		Geometry->SetVertexAttribs( VA );
		PlaneMaterialNode->Add( Geometry );

		m_Scene->Add( PlaneMaterialNode );
	}

	SpawnRandomAsteroids( 10 );
}

void clGameManager::PlayAudioFile( const std::string& FileName )
{
	auto Iter = m_SoundFiles.find( FileName );

	bool NotFound = Iter == m_SoundFiles.end();

	clPtr<clBlob> Blob = NotFound ? LoadFileAsBlob( g_FS, FileName ) : Iter->second;

	m_SoundFiles[ FileName ] = Blob;

	auto Sound = make_intrusive<clAudioSource>();
	Sound->BindWaveform( make_intrusive<clModPlugProvider>( Blob ) );
	Sound->Play();
	g_Audio.RegisterSource( g_BackgroundMusic.GetInternalPtr() );

	m_Sounds.push_back( Sound );
}

void clGameManager::FireRocket( const vec3& Pos, const vec3& Vel )
{
	auto Rocket = make_intrusive<clRocket>();
	Rocket->m_Pos = Pos;
	Rocket->m_Vel = Vel;
	Rocket->AttachToScene( m_Scene );
	m_Rockets.push_back( Rocket );

	PlayAudioFile( "Launch.wav" );
}

void clGameManager::AddExplosion( const vec3& Pos, const vec3& Dir )
{
	auto Explosion = make_intrusive<clExplosion>();
	Explosion->m_Pos = Pos;
	Explosion->m_ParticleAccel = Dir;
	Explosion->AttachToScene( m_Scene );
	m_Explosions.push_back( Explosion );

	PlayAudioFile( "Explosion.wav" );
}

bool clGameManager::IsInsideLevel( const vec3& Pos )
{
	if ( Pos.x < m_LevelMin.x ) { return false; }

	if ( Pos.y < m_LevelMin.y ) { return false; }

	if ( Pos.x > m_LevelMax.x ) { return false; }

	if ( Pos.y > m_LevelMax.y ) { return false; }

	return true;
}

vec3 clGameManager::ClampToLevel( const vec3& Pos )
{
	vec3 C = Pos;

	while ( C.x < m_LevelMin.x ) { C.x += m_LevelMax.x - m_LevelMin.x; }

	while ( C.y < m_LevelMin.y ) { C.y += m_LevelMax.y - m_LevelMin.y; }

	while ( C.x > m_LevelMax.x ) { C.x -= m_LevelMax.x - m_LevelMin.x; }

	while ( C.y > m_LevelMax.y ) { C.y -= m_LevelMax.y - m_LevelMin.y; }

	return C;
}

void clGameManager::Kill( iActor* Actor )
{
	m_DeathRow.push_back( Actor );
}

void clGameManager::GenerateTicks()
{
	const float DeltaSeconds = 0.05f;

	for ( const auto& i : m_Asteroids )
	{
		i->Update( DeltaSeconds );
	}

	for ( const auto& i : m_Rockets )
	{
		i->Update( DeltaSeconds );
	}

	for ( const auto& i : m_Explosions )
	{
		i->Update( DeltaSeconds );
	}

	m_SpaceShip->Update( DeltaSeconds );

	CheckCollisions();
	PerformExecution();

	for ( size_t i = 0; i != m_Sounds.size(); i++ )
	{
		if ( !m_Sounds[i]->IsPlaying() )
		{
			// one by one
			g_Audio.UnRegisterSource( m_Sounds[i].GetInternalPtr() );
			m_Sounds[i]->Stop();
			m_Sounds[i] = m_Sounds.back();
			m_Sounds.pop_back();
			break;
		}
	}
}

void clGameManager::CheckCollisions()
{
	for ( size_t i = 0; i != m_Rockets.size(); i++ )
	{
		for ( size_t j = 0; j != m_Asteroids.size(); j++ )
		{
			vec3 PosR = m_Rockets[i]->m_Pos;
			vec3 PosA = m_Asteroids[j]->m_Pos;
			float R   = m_Asteroids[j]->GetRadius();

			if ( ( PosR - PosA ).Length() < R )
			{
				this->Kill( m_Rockets[i].GetInternalPtr() );
				this->Kill( m_Asteroids[j].GetInternalPtr() );
				AddExplosion( m_Asteroids[j]->m_Pos, m_Rockets[i]->m_Vel );
			}
		}
	}
}

template <typename Container, typename Entity>
void Remove( Container& c, Entity e )
{
	auto iter = std::remove_if( c.begin(), c.end(), [ e ]( const typename Container::value_type & Ent ) { return Ent == e; } );
	c.erase( iter, c.end() );
}

void clGameManager::SpawnRandomAsteroids( size_t N )
{
	while ( N-- )
	{
		auto Asteroid = make_intrusive<clAsteroid>();
		Asteroid->m_Pos = Math::RandomVector3InRange( m_LevelMin, m_LevelMax );
		Asteroid->m_Vel = Math::RandomVector3InRange( vec3( -0.3f, -0.3f, 0.0f ), vec3( 0.3f, 0.3f, 0.0f ) );
		Asteroid->AttachToScene( m_Scene );
		m_Asteroids.push_back( Asteroid );
	}
}

void clGameManager::PerformExecution()
{
	for ( const auto& i : m_DeathRow )
	{
		i->DetachFromScene( m_Scene );
		Remove( m_Asteroids, i );
		Remove( m_Explosions, i );
		Remove( m_Rockets, i );
	}

	m_DeathRow.clear();
}

void UpdateTrackball( float Speed )
{
	g_Trackball.DragTo( g_MouseState.FPos, Speed, g_MouseState.FPressed );
}

void clGameManager::Render()
{
//	UpdateTrackball( 10.0f );

	mat4 TrackballMtx = mat4::Identity(); //g_Trackball.GetRotationMatrix();

	mat4 Proj = Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f );
	mat4 View = TrackballMtx * g_Camera.GetViewMatrix();

	g_Technique->Render( m_Scene, Proj, View, g_MatSys );
}

bool clGameManager::IsKeyPressed( int Code )
{
	auto i = m_Keys.find( Code );

	if ( i == m_Keys.end() ) { return false; }

	return i->second;
}

void clGameManager::OnKey( int Key, bool Pressed )
{
	m_Keys[ Key ] = Pressed;
}
