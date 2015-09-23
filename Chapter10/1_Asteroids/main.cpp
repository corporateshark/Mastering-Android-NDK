#include "Globals.h"
#include "MOD.h"
#include "tinythread.h"
#include "ParticleMaterial.h"

#include "FI_Utils.h"

#include "GamePages.h"

#if !defined( ANDROID )
#  undef main
#endif

void InitJoystick()
{
	auto ImgStream = g_FS->CreateReader( "pad.png" );
	g_JoystickImage = clBitmap::LoadImg( ImgStream );
	g_JoystickTexture = g_Canvas->CreateTexture( g_JoystickImage );

	ImgStream = g_FS->CreateReader( "pad_mask.png" );
	g_JoystickImage = clBitmap::LoadImg( ImgStream );

	g_ScreenJoystick = new ScreenJoystick();
	g_ScreenJoystick->FMaskBitmap = clBitmap::LoadImg( ImgStream );

	// Init joystick:
	float A_Y = 414.0f / 512.0f;

	sBitmapAxis B_Left;
	B_Left.FAxis1 = 0;
	B_Left.FAxis2 = 1;
	B_Left.FPosition = vec2( 55.0f / 512.f, A_Y );
	B_Left.FRadius = 40.0f / 512.0f;
	B_Left.FColour = ivec4( 192, 192, 192, 0 );

	sBitmapButton B_Fire;
	B_Fire.FIndex = 0;
	B_Fire.FColour = ivec4( 0, 0, 0, 0 );

	g_ScreenJoystick->FAxisDesc.push_back( B_Left );
	g_ScreenJoystick->FButtonDesc.push_back( B_Fire );

	g_ScreenJoystick->InitKeys();
	g_ScreenJoystick->Restart();
}

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

void OnDrawFrame()
{
	g_ScreenWidth = g_Window->GetWidth();
	g_ScreenHeight = g_Window->GetHeight();

	g_Game->GenerateTicks();
	g_Game->Render();

	// 3. Overlays
	LGL3->glClear( GL_DEPTH_BUFFER_BIT );
	LGL3->glDisable( GL_DEPTH_TEST );
	g_Canvas->SetColor( 255, 255, 255 );

	g_Canvas->TextureRect( 0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_JoystickImage->GetWidth(), g_JoystickImage->GetHeight(), g_JoystickTexture );
}

void OnKey( int KeyCode, bool Pressed )
{
	g_GUI->OnKey( g_MouseState.FPos, KeyCode, Pressed );
}

void OnTouch( int X, int Y, bool Touch )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );
	g_MouseState.FPressed = Touch;

	g_GUI->OnTouch( X, Y, Touch );
}

void OnMove( int X, int Y )
{
	g_MouseState.FPos = g_Window->GetNormalizedPoint( X, Y );

	g_GUI->OnMove( X, Y );
}

int main( int argc, char* argv[] )
{
	clSDL SDLLibrary;

	LoadFreeImage();

	g_Window = clSDLWindow::CreateSDLWindow( "Asteroids", 1024, 768 );

	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

	g_GUI = make_intrusive<clGUI>( g_Window );

#if !defined(ANDROID)
	OnStart( "." );
#endif

	LoadModPlug();

	g_Audio.Start();
	g_Audio.Wait();

	g_BackgroundMusic = make_intrusive<clAudioSource>();

	g_BackgroundMusic->BindWaveform( make_intrusive<clModPlugProvider>( LoadFileAsBlob( g_FS, "theme.mod" ) ) );
	g_BackgroundMusic->Play();
	g_Audio.RegisterSource( g_BackgroundMusic.GetInternalPtr() );

	g_Technique = make_intrusive<clForwardRenderingTechnique>();
	g_Camera.SetPosition( vec3( 0, 0, 5 ) );
	g_MatSys = make_intrusive<clParticleMaterialSystem>();

	g_Game = make_intrusive<clGameManager>();

	g_Canvas = make_intrusive<clGLCanvas>( g_Window );

	InitJoystick();

	// Construct UI pages
	g_MenuPage = make_intrusive<clMenuPage>();
	g_OptionsPage = make_intrusive<clOptionsPage>();
	g_GamePage = make_intrusive<clGamePage>();

	g_GUI->AddPage( g_MenuPage );
	g_GUI->AddPage( g_OptionsPage );
	g_GUI->AddPage( g_GamePage );

	g_GUI->SetActivePage( g_MenuPage );

	double Seconds = Env_GetSeconds();
	double AccumulatedTime = 0.0;
	const float TimeQuantum = 0.01f;

	while ( g_Window && g_Window->HandleInput() )
	{
		double NextSeconds = Env_GetSeconds();
		AccumulatedTime += ( NextSeconds - Seconds );
		Seconds = NextSeconds;

		while ( AccumulatedTime > TimeQuantum )
		{
			g_GUI->Update( TimeQuantum );

			AccumulatedTime -= TimeQuantum;
		}

		g_Canvas->SetColor( 0, 0, 0 );
		g_Canvas->Clear();

		g_Canvas->SetColor( 255, 255, 255 );

		g_ScreenWidth = g_Window->GetWidth();
		g_ScreenHeight = g_Window->GetHeight();

		g_GUI->Draw( g_Canvas );

		g_Canvas->Present();
	}

	OnStop();

	g_BackgroundMusic->Stop();
	g_BackgroundMusic = nullptr;

	g_Audio.Exit( true );

	g_Game = nullptr;
	g_MatSys = nullptr;
	g_Canvas = nullptr;
	g_Window = nullptr;
	g_Technique = nullptr;

	return 0;
}
