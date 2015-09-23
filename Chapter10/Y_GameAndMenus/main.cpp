#include <stdio.h>
#include <SDL2/SDL.h>

#include "Timers.h"
#include "Platform.h"
#include "SDLLibrary.h"
#include "SDLWindow.h"
#include "Rendering/Canvas.h"
#include "Rendering/TextRenderer.h"

//#include "UI/UIStatic.h"
#include "UI/UIButton.h"
#include "UI/LayoutController.h"

#include "FI_Utils.h"

#include "FileSystem.h"
#include "Archive.h"
#include "Log.h"
#include "Bitmap.h"
#include "LGL/LGL.h"

#include "Pages.h"

#if defined(OS_WINDOWS)
#  undef main
#endif // OS_WINDOWS

class clMenuPage;
class clScoresPage;
class clOptionsPage;
class clGamePage;

clPtr<clFileSystem> g_FS;

clPtr<clGUI> g_GUI;

clPtr<clMenuPage> g_MenuPage;
clPtr<clOptionsPage> g_OptionsPage;
clPtr<clGamePage> g_GamePage;

clPtr<clUIView> UIRoot = nullptr;

std::unique_ptr<sLGLAPI> LGL3;

void OnStart( const std::string& RootPath )
{
	LOGI( "Mounting %s", RootPath.c_str() );

	g_FS = make_intrusive<clFileSystem>();

	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );
}

clPtr<clSDLWindow> g_Window = nullptr;

vec2 g_MousePos;

void OnMove( int X, int Y )
{
	g_MousePos = g_Window->GetNormalizedPoint( X, Y );
}

void OnKey( int KeyCode, bool Pressed )
{
	g_GUI->OnKey( g_MousePos, KeyCode, Pressed );

//		g_Window->RequestExit();
}

// OnTouch method redirects
void OnTouch( int mx, int my, bool Touch )
{
//	UIRoot->OnTouch(mx, my, Touch);
	g_GUI->OnTouch( mx, my, Touch );
}

class clAsteroidsPage : public clGUIPage
{
public:
	clAsteroidsPage() {}

	clPtr<clUIButton> SetupButton( int X, int Y, int W, int H, const ivec4& Color, const std::string& Title )
	{
		auto Btn = make_intrusive<clUIButton>();
		Btn->m_FontID = clTextRenderer::Instance()->GetFontHandle( "receipt.ttf" );
		Btn->m_FontHeight = 24;
		Btn->SetWidth( W );
		Btn->SetHeight( H );
		Btn->SetPosition( X, Y );
		Btn->SetParentFractionX( 0.5f );
		Btn->SetParentFractionY( 0.5f );
		Btn->SetAlignmentV( eAlignV_Center );
		Btn->SetAlignmentH( eAlignH_Center );
		Btn->SetBackgroundColor( Color );
		Btn->SetTitle( Title );
		return Btn;
	}
};

class clOptionsPage : public clAsteroidsPage
{
public:
	clOptionsPage()
	{
		this->FFallbackPage = g_MenuPage;
	}
};

class clGamePage : public clAsteroidsPage
{
public:
	clGamePage()
	{
		FFallbackPage = g_MenuPage;

		auto OkBtn = SetupButton( 0, 50, 100, 100, ivec4( 255, 255, 255, 255 ), "OK" );
		OkBtn->SetTouchHandler(
		   []( int x, int y )
		{
			g_GUI->SetActivePage( g_MenuPage );
			return true;
		}
		);

		this->Add( OkBtn );
	}

	virtual void Draw( const clPtr<iCanvas>& C ) override
	{
		clGUIPage::Draw( C );
	}
};

class clMenuPage : public clAsteroidsPage
{
public:
	clMenuPage()
	{
		this->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );

		auto GameBtn = SetupButton( 0, 50, 200, 100, ivec4( 255, 255, 255, 255 ), "New Game" );
		GameBtn->SetTouchHandler(
		   []( int x, int y )
		{
			g_GUI->SetActivePage( g_GamePage );
			return true;
		}
		);

		auto OptionsBtn = SetupButton( 0, 250, 200, 100, ivec4( 255, 255, 255, 255 ), "Options" );
		OptionsBtn->SetTouchHandler(
		   []( int x, int y )
		{
			g_GUI->SetActivePage( g_OptionsPage );
			return true;
		}
		);

		auto ExitBtn = SetupButton( 0, 400, 200, 100, ivec4( 255, 255, 255, 255 ), "Exit" );
		ExitBtn->SetTouchHandler(
		   []( int x, int y )
		{
			LOGI( "Exiting" );
			g_Window->RequestExit();
			return true;
		}
		);

		auto Background = make_intrusive<clUIStatic>();
		Background->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
		Background->SetBackgroundColor( ivec4( 0 ) );
		Background->SetBitmap( clBitmap::LoadImg( g_FS->CreateReader( "background.png" ) ) );

		auto VerticalLayout = make_intrusive<clVerticalLayout>();
		VerticalLayout->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );

		this->Add( Background );
		this->Add( VerticalLayout );

		{
			auto Container = make_intrusive<clUIView>();
			Container->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
			Container->Add( GameBtn );
			VerticalLayout->Add( Container );
		}
		{
			auto Container = make_intrusive<clUIView>();
			Container->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
			Container->Add( OptionsBtn );
			VerticalLayout->Add( Container );
		}
		{
			auto Container = make_intrusive<clUIView>();
			Container->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
			Container->Add( ExitBtn );
			VerticalLayout->Add( Container );
		}
	}
};

int main( int argc, char** argv )
{
	clSDL SDLLibrary;

	LoadFreeImage();

	g_Window = clSDLWindow::CreateSDLWindow( "SDL2UI", 1024, 768 );

	auto Canvas = make_intrusive<clGLCanvas>( g_Window );

	UIRoot = make_intrusive<clUIView>();

	g_GUI = make_intrusive<clGUI>( g_Window );

#if !defined(ANDROID)
	OnStart( "." );
#endif

	// Construct UI pages
	g_MenuPage = make_intrusive<clMenuPage>();
	g_OptionsPage = make_intrusive<clOptionsPage>();
	g_GamePage = make_intrusive<clGamePage>();

	g_GUI->AddPage( g_MenuPage );
	g_GUI->AddPage( g_OptionsPage );
	g_GUI->AddPage( g_GamePage );

	g_GUI->SetActivePage( g_MenuPage );

//	UIRoot->Add( Background );
//	UIRoot->Add( VerticalLayout );
//	UIRoot->Add( ExitBtn );
	//

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

		Canvas->SetColor( 0, 0, 0 );
		Canvas->Clear();

		Canvas->SetColor( 255, 255, 255 );

		g_GUI->Draw( Canvas );
		//    UIRoot->UpdateScreenPositions();
		//    UIRoot->Draw( Canvas );

		Canvas->Present();
	}

	Canvas = nullptr;
	g_Window = nullptr;

	return 0;
}
