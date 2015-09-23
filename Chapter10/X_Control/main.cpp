#include "Timers.h"
#include "FileSystem.h"
#include "SDLLibrary.h"
#include "Traversers.h"
#include "Camera.h"
#include "Canvas.h"
#include "Trackball.h"
#include "Log.h"
#include "Loader_OBJ.h"
#include "Boids.h"
#include "Bitmap.h"

#include "ScreenJoystick.h"

#include "FI_Utils.h"
bool FreeImage_SaveBitmapToFile( const std::string& FileName, uint8_t* ImageData, int W, int H, int BitsPP );

#if !defined( ANDROID )
#  undef main
#endif

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clSDLWindow> g_Window;
clPtr<clFileSystem> g_FS;
clPtr<iCanvas> g_Canvas;

clPtr<clMaterialSystem> g_MatSys;

clVirtualTrackball g_Trackball;

clPtr<clBitmap> g_JoystickImage;
int g_JoystickTexture;

clPtr<ScreenJoystick> g_ScreenJoystick;

auto g_Behaviour = make_intrusive<clGoToBehaviour>();

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
}

clTransformUpdateTraverser g_TransformUpdateTraverser;
clROPsTraverser g_ROPsTraverser;
clPtr<clSceneNode> g_Scene;

clPtr<clGeometryNode> g_Box;
clPtr<clLightNode> g_LightNode;
clPtr<clGeometryNode> g_TargetNode;
clCamera g_Camera;

void RenderROPs( sMatrices& Matrices, const std::vector<clRenderOp>& RenderQueue, ePass Pass )
{
	for ( const auto& ROP : RenderQueue )
	{
		ROP.Render( Matrices, g_MatSys, Pass );
	}
}

void UpdateShadowMaps( const std::vector<clLightNode*>& Lights, const std::vector<clRenderOp>& ROPs )
{
	LGL3->glDisable( GL_BLEND );

	for ( size_t i = 0; i != Lights.size(); i++ )
	{
		sMatrices ShadowMatrices;

		clPtr<iLight> L = Lights[ i ]->GetLight();

		clPtr<clGLFrameBuffer> ShadowBuffer = L->GetShadowMap();

		ShadowBuffer->Bind( 0 );
		LGL3->glClearColor( 0, 0, 0, 1 );
		LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		LMatrix4 Proj = L->GetProjectionForShadowMap();
		LMatrix4 MV = L->GetViewForShadowMap();

		ShadowMatrices.m_ViewMatrix = MV;
		ShadowMatrices.m_ProjectionMatrix = Proj;
		ShadowMatrices.UpdateMatrices();
		L->UpdateROPUniforms( ROPs, g_MatSys, Lights[i] );

		RenderROPs( ShadowMatrices, ROPs, ePass_Shadow );

		ShadowBuffer->UnBind();
	}
}

void OnTouch( int X, int Y, bool Touch )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );
	g_MouseState.FPressed = Touch;

	if ( !Touch )
	{
		vec3 Pos = Math::UnProjectPoint(
		              vec3( g_MouseState.FPos ),
		              Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f ),
		              g_Camera.GetViewMatrix() );

		mat4 CamRotation;
		vec3 CamPosition;
		DecomposeCameraTransformation( g_Camera.GetViewMatrix(), CamPosition, CamRotation );

		vec3 isect;
		bool R = IntersectRayToPlane( CamPosition, Pos - CamPosition, vec3( 0, 0, 1 ), 0, isect );

//		g_Behaviour->m_Target = isect.ToVector2();

//		g_ScreenJoystick->HandleTouch(L_MOTION_START, g_MouseState.FPos, true, L_MOTION_DOWN);
//		g_ScreenJoystick->HandleTouch(0, g_MouseState.FPos, true, L_MOTION_DOWN);

		g_ScreenJoystick->HandleTouch( L_MOTION_START, g_MouseState.FPos, false, L_MOTION_DOWN );
		g_ScreenJoystick->HandleTouch( 0, g_MouseState.FPos, false, L_MOTION_DOWN );
		g_ScreenJoystick->HandleTouch( L_MOTION_END, vec2(), false, L_MOTION_DOWN );
	}
	else
	{
//		g_ScreenJoystick->HandleTouch(0, g_MouseState.FPos, false, L_MOTION_UP);
//		g_ScreenJoystick->HandleTouch(L_MOTION_END, g_MouseState.FPos, false, L_MOTION_UP);

		g_ScreenJoystick->HandleTouch( L_MOTION_START, g_MouseState.FPos, false, L_MOTION_UP );
		g_ScreenJoystick->HandleTouch( 0, g_MouseState.FPos, false, L_MOTION_UP );
		g_ScreenJoystick->HandleTouch( L_MOTION_END, vec2(), false, L_MOTION_UP );
	}
}

void OnMove( int X, int Y )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );

	if ( g_MouseState.FPressed )
	{
		g_ScreenJoystick->HandleTouch( L_MOTION_START, LVector2(), false, L_MOTION_MOVE );
		g_ScreenJoystick->HandleTouch( 0, g_MouseState.FPos, g_MouseState.FPressed, L_MOTION_MOVE );
		g_ScreenJoystick->HandleTouch( L_MOTION_END, LVector2(), false, L_MOTION_MOVE );

//		g_ScreenJoystick->HandleTouch(0, g_MouseState.FPos, g_MouseState.FPressed, L_MOTION_MOVE);
	}
}

void UpdateTrackball( float Speed )
{
	g_Trackball.DragTo( g_MouseState.FPos, Speed, g_MouseState.FPressed );
}

void OnDrawFrame()
{
	UpdateTrackball( 10.0f );

	//mat4 TrackballMtx = g_Trackball.GetRotationMatrix( );
	mat4 TrackballMtx = mat4::Identity();

	g_ScreenWidth = g_Window->GetWidth();
	g_ScreenHeight = g_Window->GetHeight();

	static float Angle = 0.0f;
	static double TimeStamp = Env_GetSeconds();
	double NewTimeStamp = Env_GetSeconds();
	float DeltaSeconds = static_cast<float>( NewTimeStamp - TimeStamp );
	TimeStamp = NewTimeStamp;

	Angle += DeltaSeconds * 1.5f;

//	LOGI( "FPS = %f", DeltaSeconds > 0.0f ? 1.0f / DeltaSeconds : 1.0f );

	g_TransformUpdateTraverser.Traverse( g_Scene );
	g_ROPsTraverser.Traverse( g_Scene );

	const auto& RenderQueue = g_ROPsTraverser.GetRenderQueue();

	sMatrices Matrices;
	Matrices.m_ProjectionMatrix = Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f );
	Matrices.m_ViewMatrix = TrackballMtx * g_Camera.GetViewMatrix();
	Matrices.UpdateMatrices();

	LGL3->glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	LGL3->glEnable( GL_DEPTH_TEST );

	// 1. Render ambient lighting and fill the Z-buffer
	LGL3->glDisablei( GL_BLEND, 0 );
	LGL3->glDepthFunc( GL_LEQUAL );

	RenderROPs( Matrices, RenderQueue, ePass_Ambient );

	auto Lights = g_ROPsTraverser.GetLights();

	UpdateShadowMaps( Lights, RenderQueue );

	// 2. Add per-light lighting
	LGL3->glDepthFunc( GL_EQUAL );
	LGL3->glBlendFunc( GL_ONE, GL_ONE );
	LGL3->glEnablei( GL_BLEND, 0 );

	for ( const auto& L : Lights )
	{
		L->GetLight()->UpdateROPUniforms( RenderQueue, g_MatSys, L );

		RenderROPs( Matrices, RenderQueue, ePass_Light );
	}

	// 3. Overlays
	LGL3->glClear( GL_DEPTH_BUFFER_BIT );
	LGL3->glDisable( GL_DEPTH_TEST );
	g_Canvas->SetColor( 255, 255, 255 );

	g_Canvas->TextureRect( 0, 0, g_ScreenWidth, g_ScreenHeight, g_JoystickTexture );

	g_Canvas->Rect( 0, 0, 100, 100, true );
}

class clSwarmRenderer
{
public:
	explicit clSwarmRenderer( const clPtr<clSwarm> Swarm )
		: m_Root( make_intrusive<clSceneNode>() )
		, m_Swarm( Swarm )
	{
		m_Boids.reserve( Swarm->m_Boids.size() );

		const float Size = 0.05f;

		for ( const auto& i : Swarm->m_Boids )
		{
			m_Boids.emplace_back( make_intrusive<clSceneNode>() );
			auto VA = clGeomServ::CreateTriangle( -0.5f * Size, Size, Size, 0.0f );
			auto GeometryNode = make_intrusive<clGeometryNode>( );
			GeometryNode->SetVertexAttribs( VA );
			m_Boids.back()->Add( GeometryNode );
			m_Root->Add( m_Boids.back() );
		}

		Update();
	}

	void Update()
	{
		for ( size_t i = 0; i != m_Boids.size(); i++ )
		{
			float Angle = m_Swarm->m_Boids[i]->m_Angle;
			//Angle = 0.0f;
			mat4 T = mat4::GetTranslateMatrix( vec3( m_Swarm->m_Boids[i]->m_Pos ) );
			mat4 R = mat4::GetRotateMatrixAxis( Angle, vec3( 0, 0, 1 ) );
			m_Boids[i]->SetLocalTransform( R * T );
		}
	}

	clPtr<clSceneNode> GetRootNode() const { return m_Root; }

private:
	clPtr<clSceneNode> m_Root;
	clPtr<clSwarm> m_Swarm;
	std::vector< clPtr<clSceneNode> > m_Boids;
};

vec2 g_pos;
float g_angle;

float g_rotation;
float g_speed;
float g_accel;

vec2 g_max, g_min;

void OnTimer( float Delta )
{
	g_rotation = g_ScreenJoystick->GetAxisValue( 0 );
	g_accel = g_ScreenJoystick->GetAxisValue( 1 )
	          < -0.5f // g_ScreenJoystick->IsPressed( 0 )
	          ? 10.0f : 0.0f;

	if ( g_rotation > -0.4f && g_rotation < 0.4f )
	{
		g_rotation = 0.0f;
	}

	float g_rotationfactor = 3.0f;

	g_rotation *= g_rotationfactor;

	g_angle += g_rotation * Delta;
	float dd = g_speed * Delta;
	vec2 dir( cosf( g_angle ), sinf( g_angle ) );
	g_pos += dd * dir;

	if ( g_accel > 0.0f ) { g_speed += g_accel * Delta; }
	else { g_speed *= 0.5f; }

	float g_maxspeed = 3.0f;

	if ( g_speed > g_maxspeed ) { g_speed = g_maxspeed; }

	if ( g_pos.x > g_max.x ) { g_pos.x = g_max.x; }

	if ( g_pos.y > g_max.y ) { g_pos.y = g_max.y; }

	if ( g_pos.x < g_min.x ) { g_pos.x = g_min.x; }

	if ( g_pos.y < g_min.y ) { g_pos.y = g_min.y; }

	//printf("g_pos = %f, %f;  g_angle = %f\n", g_pos.x, g_pos.y, g_angle);
	printf( "rotation = %f, accel = %f, a1 = %f, a2 = %f\n",
	        g_rotation, g_accel,
	        g_ScreenJoystick->GetAxisValue( 0 ),
	        g_ScreenJoystick->GetAxisValue( 1 ) );

	g_Behaviour->m_Target = g_pos;
}

int main( int argc, char* argv[] )
{
	clSDL SDLLibrary;

	LoadFreeImage();

	g_Window = clSDLWindow::CreateSDLWindow( "SceneGraph Rendering", 1024, 768 );

	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

#if !defined(ANDROID)
	OnStart( "." );
#endif

	g_Camera.SetPosition( vec3( 0, 0, 5 ) );

	g_MatSys = make_intrusive<clMaterialSystem>();
	g_Scene = make_intrusive<clSceneNode>();
	g_Canvas = make_intrusive<clGLCanvas>( g_Window );

	auto ImgStream = g_FS->CreateReader( "pad.png" );
	g_JoystickImage = clBitmap::LoadImg( ImgStream );

//	FreeImage_SaveBitmapToFile("pad_out.png", g_JoystickImage->FBitmapData, g_JoystickImage->GetWidth(), g_JoystickImage->GetHeight(), 32);
	g_JoystickTexture = g_Canvas->CreateTexture( g_JoystickImage );

	ImgStream = g_FS->CreateReader( "pad_mask.png" );
	g_JoystickImage = clBitmap::LoadImg( ImgStream );

	g_ScreenJoystick = new ScreenJoystick();
	g_ScreenJoystick->FMaskBitmap = clBitmap::LoadImg( ImgStream );


	//// Controller params
	g_angle = 0.0f;

	g_accel = g_speed = 0.0f;
	g_rotation = 0.0f;

	g_pos = vec2( 0.0f, 0.0f );

	g_max = vec2( 10.0f, 10.0f );
	g_min = -g_max;

	/** */
	// Init joystick:
	float A_Y = 414.0f / 512.0f;

	sBitmapAxis B_Left;
	B_Left.FAxis1 = 0;
	B_Left.FAxis2 = 1;
	B_Left.FPosition = vec2( 55.0f / 512.f, A_Y );
	B_Left.FRadius = 40.0f / 512.0f;
	B_Left.FColour = ivec4( 192, 192, 192, 0.0f );

	sBitmapButton B_Fire;
	B_Fire.FIndex = 0;
	B_Fire.FColour = ivec4( 0, 0, 0, 0 );

	g_ScreenJoystick->FAxisDesc.push_back( B_Left );
	g_ScreenJoystick->FButtonDesc.push_back( B_Fire );

	g_ScreenJoystick->InitKeys();
	g_ScreenJoystick->Restart();
	/** */


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

	// light 1
	{
		auto Light = make_intrusive<iLight>();
		Light->m_Diffuse = vec4( 0.5f, 0.5f, 0.5f, 1.0f );
		Light->m_Position = vec3( 5, 5, 5 );
		Light->m_Direction = vec3( -1, -1, -1 ).GetNormalized();
		Light->m_SpotOuterAngle = 21;
		g_LightNode = make_intrusive<clLightNode>();
		g_LightNode->SetLight( Light );
		g_Scene->Add( g_LightNode );
	}

	// assemble
	g_Scene->Add( CubeMaterialNode );
	g_Scene->Add( PlaneMaterialNode );

	auto Swarm = make_intrusive<clSwarm>();
	Swarm->GenerateRandom( 10 );

	auto MixedControl = make_intrusive<clMixedBehaviour>();
	MixedControl->AddBehaviour( 0.5f, make_intrusive<clFlockingBehaviour>() );
	MixedControl->AddBehaviour( 0.5f, g_Behaviour );

	Swarm->SetSingleBehaviour( MixedControl /*g_Behaviour*/ );
	g_Behaviour->m_TargetRadius = 0.1f;
	g_Behaviour->m_Target = vec2( 1.0f );
	g_Behaviour->m_PosGain = 0.1f;
	clSwarmRenderer SwarmRenderer( Swarm );

	{
		auto VA = clGeomServ::CreateAxisAlignedBox( vec3( -g_Behaviour->m_TargetRadius ), vec3( +g_Behaviour->m_TargetRadius ) );
		g_TargetNode = make_intrusive<clGeometryNode>();
		g_TargetNode->SetVertexAttribs( VA );
		PlaneMaterialNode->Add( g_TargetNode );
	}

	CubeMaterialNode->Add( SwarmRenderer.GetRootNode( ) );

	double Seconds = Env_GetSeconds();
	double AccumulatedTime = 0.0;
	const float TimeQuantum = 0.01f;

	while ( g_Window && g_Window->HandleInput() )
	{
		double NextSeconds = Env_GetSeconds();
		AccumulatedTime += ( NextSeconds - Seconds );
		Seconds = NextSeconds;

		OnDrawFrame();

		g_Window->Swap();

		while ( AccumulatedTime > TimeQuantum )
		{
			Swarm->Update( TimeQuantum );
			OnTimer( TimeQuantum );

			AccumulatedTime -= TimeQuantum;
		}


		mat4 TargetT = mat4::GetTranslateMatrix( vec3( g_Behaviour->m_Target.x, g_Behaviour->m_Target.y, -1.0f ) );
		mat4 TargetR = mat4::GetRotateMatrixAxis( g_angle, vec3( 0, 0, 1 ) );
		g_TargetNode->SetLocalTransform( TargetR * TargetT );

		/*
		g_TargetNode->SetLocalTransform(
		   mat4::GetTranslateMatrix(vec3(g_Behaviour->m_Target.x, g_Behaviour->m_Target.y, -1.0f ))
		);*/

		SwarmRenderer.Update();
	}

	OnStop();

	g_Canvas = nullptr;
	g_Window = nullptr;

	return 0;
}
