#include <stdio.h>
#include <SDL2/SDL.h>

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

#if defined(OS_WINDOWS)
#  undef main
#endif // OS_WINDOWS

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clFileSystem> g_FS;

clPtr<clUIView> UIRoot = nullptr;

void OnStart( const std::string& RootPath )
{
	LOGI( "Mounting %s", RootPath.c_str() );

	g_FS = make_intrusive<clFileSystem>();

	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );
}

bool FreeImage_SaveBitmapToFile( const std::string& FileName, uint8_t* ImageData, int W, int H, int BitsPP );

clPtr<clSDLWindow> g_Window = nullptr;

void OnMove( int X, int Y )
{
}

void OnKey( int KeyCode, bool Pressed )
{
	g_Window->RequestExit();
}

// OnTouch method redirects
void OnTouch( int mx, int my, bool Touch )
{
	UIRoot->OnTouch( mx, my, Touch );
}

class clGUI;

/// GUI page - similar to GUI window but always occupies the whole screen and only one page can be active at a time
class clGUIPage: public clUIView
{
public:
	clGUIPage(): FFallbackPage( NULL ) {}
	virtual ~clGUIPage() {}

	virtual void Update( float DeltaTime ) {}

	virtual bool OnTouch( int x, int y, bool Pressed );

	virtual void Update( double Delta );

	virtual void SetActive();

	virtual void OnActivation() {}
	virtual void OnDeactivation() {}
public:
	// GUI events (standart Esc press behaviour)
	virtual bool OnKey( int Key, bool KeyState );

public:
	/// The page we return to when the Esc press occurs
	clPtr<clGUIPage> FFallbackPage;

	/// Link to the GUI
	clGUI* FGUI;
};


class clGUI: public iIntrusiveCounter
{
public:
	clGUI(): FActivePage( NULL ), FPages() {}
	virtual ~clGUI() {}
	void AddPage( const clPtr<clGUIPage>& P )
	{
		P->FGUI = this;
		FPages.push_back( P );
	}
	void SetActivePage( const clPtr<clGUIPage>& Page )
	{
		if ( Page == FActivePage ) { return; }

		if ( FActivePage )
		{
			FActivePage->OnDeactivation();
		}

		if ( Page )
		{
			Page->OnActivation();
		}

		FActivePage = Page;
	}
	void Update( float DeltaTime )
	{
		if ( FActivePage )
		{
			FActivePage->Update( DeltaTime );
		}
	}
	void Draw( const clPtr<iCanvas>& C )
	{
		if ( FActivePage )
		{
			FActivePage->Draw( C );
		}
	}
	void OnKey( vec2 MousePos, int Key, bool KeyState )
	{
		FMousePosition = MousePos;

		if ( FActivePage )
		{
			FActivePage->OnKey( Key, KeyState );
		}
	}
	void OnTouch( int x, int y, bool TouchState )
	{
		if ( FActivePage )
		{
			FActivePage->OnTouch( x, y, TouchState );
		}
	}
private:
	vec2 FMousePosition;
	clPtr<clGUIPage> FActivePage;
	std::vector< clPtr<clGUIPage> > FPages;
};

bool clGUIPage::OnKey( int Key, bool KeyState )
{
	if ( !KeyState && Key == 27 )
	{
		/// Call custom handler to allow some actions like haptic feedback
		if ( FFallbackPage.GetInternalPtr() != NULL )
		{
			FGUI->SetActivePage( FFallbackPage );
			return true;
		}
	}

	return false;
}

void clGUIPage::SetActive()
{
	FGUI->SetActivePage( this );
}


int main( int argc, char** argv )
{
	clSDL SDLLibrary;

	LoadFreeImage();

	g_Window = clSDLWindow::CreateSDLWindow( "SDL2UI", 1024, 768 );

	auto Canvas = make_intrusive<clGLCanvas>( g_Window );

	UIRoot = make_intrusive<clUIView>();

	g_Window->SetRootView( UIRoot );

#if !defined(ANDROID)
	OnStart( "." );
#endif

	// construct a simple demo UI

	/*
	   UIRoot
	      |
	      |- Background
	      |- VerticalLayout
	         |
	         |- TopBar
	         |- HorizontalLayout
	         |     |- Panel1
	         |     |- Panel2
	         |
	         |- CommandLine
	         |- BottomBar
	      |
	      |- MsgBox

	*/
	auto Background = make_intrusive<clUIStatic>();
	Background->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	Background->SetBackgroundColor( ivec4( 0 ) );

	auto VerticalLayout = make_intrusive<clVerticalLayout>();
	VerticalLayout->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	auto HorizontalLayout = make_intrusive<clHorizontalLayout>();
	HorizontalLayout->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );

	auto Panel1 = make_intrusive<clUIStatic>();
	Panel1->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	Panel1->SetBackgroundColor( ivec4( 0, 0, 255, 255 ) );
	auto Panel2 = make_intrusive<clUIStatic>();
	Panel2->SetBackgroundColor( ivec4( 0, 0, 128, 255 ) );
	Panel2->SetFillMode( eFillMode_Horizontal | eFillMode_Vertical );
	HorizontalLayout->Add( Panel1 );
	HorizontalLayout->Add( Panel2 );

	auto TopBar = make_intrusive<clUIStatic>();
	TopBar->SetFillMode( eFillMode_Horizontal );
	TopBar->SetHeight( 50 );
	TopBar->SetBackgroundColor( ivec4( 255, 0, 0, 255 ) );
	VerticalLayout->Add( TopBar );
	VerticalLayout->Add( HorizontalLayout );
	auto CommandLine = make_intrusive<clUIStatic>();
	CommandLine->SetFillMode( eFillMode_Horizontal );
	CommandLine->SetHeight( 50 );
	CommandLine->SetBackgroundColor( ivec4( 0, 255, 0, 255 ) );
	VerticalLayout->Add( CommandLine );
	auto BottomBar = make_intrusive<clUIStatic>();
	BottomBar->SetFillMode( eFillMode_Horizontal );
	BottomBar->SetHeight( 50 );
	BottomBar->SetBackgroundColor( ivec4( 255, 255, 0, 255 ) );
	VerticalLayout->Add( BottomBar );

	auto ExitBtn = make_intrusive<clUIButton>();
	ExitBtn->m_FontID = clTextRenderer::Instance()->GetFontHandle( "receipt.ttf" );
	ExitBtn->m_FontHeight = 24;
	ExitBtn->SetParentFractionX( 0.5f );
	ExitBtn->SetParentFractionY( 0.5f );
	ExitBtn->SetAlignmentV( eAlignV_Center );
	ExitBtn->SetAlignmentH( eAlignH_Center );
	ExitBtn->SetBackgroundColor( ivec4(  255, 255, 255, 255 ) );
	ExitBtn->SetTitle( "Exit" );

	ExitBtn->SetTouchHandler(
	   []( int x, int y )
		{
			LOGI( "Exiting" );

			g_Window->RequestExit();
			return true;
		}
	);

	UIRoot->Add( Background );
	UIRoot->Add( VerticalLayout );
	UIRoot->Add( ExitBtn );
	//

	while ( g_Window && g_Window->HandleInput() )
	{
		Canvas->SetColor( 0, 0, 0 );
		Canvas->Clear();

		UIRoot->UpdateScreenPositions();
		UIRoot->Draw( Canvas );

		Canvas->SetColor( 255, 255, 255 );

		Canvas->Present();
	}

	Canvas = nullptr;
	g_Window = nullptr;

	return 0;
}
