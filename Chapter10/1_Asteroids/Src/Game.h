#pragma once

#include <unordered_map>
#include <vector>
#include <map>

#include "IntrusivePtr.h"
#include "Actors.h"
#include "Nodes.h"

class clBlob;
class clAudioSource;

class clGameManager: public iIntrusiveCounter
{
public:
	clGameManager();

	virtual void GenerateTicks();
	virtual void Render();
	virtual void OnKey( int Key, bool Pressed );

	clPtr<clSceneNode> GetSceneRoot() const { return m_Scene; };

	virtual bool IsKeyPressed( int Code );

	virtual void FireRocket( const vec3& Pos, const vec3& Vel );
	virtual bool IsInsideLevel( const vec3& Pos );
	virtual vec3 ClampToLevel( const vec3& Pos );
	virtual void Kill( iActor* Actor );
	virtual void AddExplosion( const vec3& Pos, const vec3& Dir );
	virtual void PlayAudioFile( const std::string& FileName );

private:
	void PerformExecution();
	void SpawnRandomAsteroids( size_t N );
	void CheckCollisions();

private:
	clPtr<clSceneNode> m_Scene;
	clPtr<clSpaceShip> m_SpaceShip;
	std::vector< clPtr<clAsteroid> > m_Asteroids;
	std::vector< clPtr<clRocket> > m_Rockets;
	std::vector< clPtr<clExplosion> > m_Explosions;
	std::unordered_map<int, bool> m_Keys;
	vec3 m_LevelMin;
	vec3 m_LevelMax;
	std::vector< iActor* > m_DeathRow;
	std::vector< clPtr<clAudioSource> > m_Sounds;
	// file name -> blob
	std::map< std::string, clPtr<clBlob> > m_SoundFiles;
};
