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

#include "Timers.h"
#include "FileSystem.h"
#include "Blob.h"
#include "SDLLibrary.h"
#include "SDLWindow.h"
#include "Nodes.h"
#include "Traversers.h"
#include "Camera.h"
#include "Canvas.h"
#include "Trackball.h"
#include "Log.h"
#include "Loader_OBJ.h"
#include "LGL/LGL.h"
#include "Technique.h"
#include "ParticleNode.h"
#include "ParticleMaterial.h"

#if !defined( ANDROID )
#  undef main
#endif

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clSDLWindow> g_Window;
clPtr<clFileSystem> g_FS;
clPtr<iCanvas> g_Canvas;

clPtr<clMaterialSystem> g_MatSys;

clVirtualTrackball g_Trackball;

bool g_UpdateParticles = true;

struct sMouseState
{
	vec2 FPos;
	bool FPressed;
} g_MouseState;

void OnStart( const std::string& RootPath )
{
	g_FS = make_intrusive<clFileSystem>();
	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );
}

void OnStop()
{
}

void OnKey( int KeyCode, bool Pressed )
{
#if !defined(ANDROID)

	if ( KeyCode == SDLK_ESCAPE )
#endif
	{
		g_Window->RequestExit();
	}

	if ( Pressed && KeyCode == SDLK_SPACE )
	{
		g_UpdateParticles = !g_UpdateParticles;
	}
}

clPtr<clForwardRenderingTechnique> g_Technique;
clPtr<clSceneNode> g_Scene;

clPtr<clGeometryNode> g_Box;
clPtr<clLightNode> g_LightNode;
clCamera g_Camera;

void OnTouch( int X, int Y, bool Touch )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );
	g_MouseState.FPressed = Touch;
}

void OnMove( int X, int Y )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );
}

void UpdateTrackball( float Speed )
{
	g_Trackball.DragTo( g_MouseState.FPos, Speed, g_MouseState.FPressed );
}

void OnDrawFrame()
{
	UpdateTrackball( 10.0f );

	mat4 TrackballMtx = g_Trackball.GetRotationMatrix( );

	g_ScreenWidth = g_Window->GetWidth();
	g_ScreenHeight = g_Window->GetHeight();

	static float Angle = 0.0f;

	g_Box->SetLocalTransform(
	   mat4::GetRotateMatrixAxis( Angle, vec3( 0, 1, 1 ) ) *
	   mat4::GetRotateMatrixAxis( 30.0f * Math::DTOR, vec3( 1, 1, 1 ) )
	);

	static double TimeStamp = Env_GetSeconds();
	double NewTimeStamp = Env_GetSeconds();
	float DeltaSeconds = static_cast<float>( NewTimeStamp - TimeStamp );
	TimeStamp = NewTimeStamp;

	Angle += DeltaSeconds * 1.5f;

	LOGI( "FPS = %f", DeltaSeconds > 0.0f ? 1.0f / DeltaSeconds : 1.0f );

	mat4 Proj = Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f );
	mat4 View = TrackballMtx * g_Camera.GetViewMatrix();

	g_Technique->Render( g_Scene, Proj, View, g_MatSys );
}

void GenerateExplosion( const clPtr<clParticleSystemNode>& ParticleNode, const vec3& Pos )
{
	if ( ParticleNode->GetParticleSystem()->GetParticles().size() > 8000 ) { return; }

	const vec4 Pal[] =
	{
		vec4( 0.2f, 0.30f, 0.8f, 1.0f ),
		vec4( 0.7f, 0.25f, 0.3f, 1.0f ),
		vec4( 0.1f, 0.80f, 0.2f, 1.0f )
	};

	vec4 Color = Pal[ Math::RandomInRange( 0, 3 ) ];

	auto Emitter = make_intrusive<clParticleEmitter_Explosion>();
	Emitter->FCenter = Pos;
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
	Emitter->FAcceleration = vec3( 0.0f, 0.0f, -3.0f );

	ParticleNode->AddEmitter( Emitter );
}

int main( int argc, char* argv[] )
{
	clSDL SDLLibrary;

	g_Window = clSDLWindow::CreateSDLWindow( "SceneGraph Rendering", 1024, 768 );

	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

#if !defined(ANDROID)
	OnStart( "." );
#endif

	g_Technique = make_intrusive<clForwardRenderingTechnique>();

	g_Camera.SetPosition( vec3( 0, 0, 5 ) );

	g_MatSys = make_intrusive<clParticleMaterialSystem>();
	g_Scene = make_intrusive<clSceneNode>();
	g_Canvas = make_intrusive<clGLCanvas>( g_Window );

	auto CubeMaterialNode = make_intrusive<clMaterialNode>();
	{
		sMaterial Material;
		Material.m_Ambient = vec4( 0.2f, 0.0f, 0.0f, 1.0f );
		Material.m_Diffuse = vec4( 0.8f, 0.0f, 0.0f, 1.0f );
		CubeMaterialNode->SetMaterial( Material );
	}
	auto PlaneMaterialNode = make_intrusive<clMaterialNode>();
	{
		sMaterial Material;
		Material.m_Ambient = vec4( 0.0f, 0.2f, 0.0f, 1.0f );
		Material.m_Diffuse = vec4( 0.0f, 0.8f, 0.0f, 1.0f );
		PlaneMaterialNode->SetMaterial( Material );
	}

	// big box
	{
		auto VA = clGeomServ::CreateAxisAlignedBox( vec3( -0.5 ), vec3( +0.5 ) );
		g_Box = make_intrusive<clGeometryNode>();
		g_Box->SetVertexAttribs( VA );
		CubeMaterialNode->Add( g_Box );
	}

	// plane
	{
		auto VA = clGeomServ::CreateAxisAlignedBox( vec3( -2.0f, -2.0f, -1.0f ), vec3( 2.0f, 2.0f, -0.95f ) );
		auto Geometry = make_intrusive<clGeometryNode>();
		Geometry->SetVertexAttribs( VA );
		PlaneMaterialNode->Add( Geometry );
	}

	// particle system
	auto ParticleNode = make_intrusive<clParticleSystemNode>();
	{
		GenerateExplosion( ParticleNode, vec3( 0 ) );
		/*
		auto Emitter = make_intrusive<clParticleEmitter_Sphere>();
		Emitter->FCenter = vec3( 0.0f );
		Emitter->FRadius = 2.0f;
		Emitter->FSizeMin = 0.1f;
		Emitter->FSizeMax = 0.2f;
		Emitter->FLifetimeMin = 0.1f;
		Emitter->FLifetimeMax = 0.5f;
		Emitter->FMaxParticles = 10000;
		Emitter->FEmissionRate = 3000;
		Emitter->FRadialVelocityMin = 1.0f;
		Emitter->FRadialVelocityMax = 5.0f;

		ParticleNode->AddEmitter( Emitter );
		*/

		g_Scene->Add( ParticleNode );
	}

	// light 1
	{
		auto Light = make_intrusive<iLight>( );
		Light->m_Diffuse = vec4( 0.5f, 0.5f, 0.5f, 1.0f );
		Light->m_Position = vec3( 5, 5, 5 );
		Light->m_Direction = vec3( -1, -1, -1 ).GetNormalized();
		Light->m_SpotOuterAngle = 21;
		g_LightNode = make_intrusive<clLightNode>( );
		g_LightNode->SetLight( Light );
		g_Scene->Add( g_LightNode );
	}

	// assemble
//	g_Scene->Add( CubeMaterialNode );
	g_Scene->Add( PlaneMaterialNode );

	double Seconds = Env_GetSeconds();

	while ( g_Window && g_Window->HandleInput() )
	{
		double NextSeconds = Env_GetSeconds();
		float DeltaTime = static_cast<float>( NextSeconds - Seconds );
		Seconds = NextSeconds;

		float SlowMotionCoef = 0.5f;

		if ( g_UpdateParticles ) { ParticleNode->UpdateParticles( SlowMotionCoef * DeltaTime ); }

		LOGI( "Num particles = %u", ParticleNode->GetParticleSystem()->GetParticles().size() );
		bool Add = Math::RandomInRange( 0, 100 ) > 50.0f;

		if ( !ParticleNode->GetParticleSystem()->GetParticles().size() || Add )
		{
			GenerateExplosion( ParticleNode, Math::RandomVector3InRange( vec3( -1 ), vec3( +1 ) ) );
		}

		OnDrawFrame();

		g_Window->Swap();
	}

	OnStop();

	g_Canvas = nullptr;
	g_Window = nullptr;
	g_Technique = nullptr;

	return 0;
}
