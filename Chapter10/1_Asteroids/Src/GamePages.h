#pragma once

#include "UI/LayoutController.h"
#include "UI/UIButton.h"
#include "Pages.h"
#include "Bitmap.h"

#include "Globals.h"

#include "Game.h"

extern clPtr<clGUI> g_GUI;

class clMenuPage;
class clOptionsPage;
class clGamePage;

extern clPtr<clMenuPage> g_MenuPage;
extern clPtr<clOptionsPage> g_OptionsPage;
extern clPtr<clGamePage> g_GamePage;

class clAsteroidsPage : public clGUIPage
{
public:
	clAsteroidsPage() {}

	clPtr<clUIButton> SetupButton( int X, int Y, int W, int H, const ivec4& Color, const std::string& Title );
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

	virtual void OnMove( int X, int Y ) override
	{
		if ( g_MouseState.FPressed )
		{
			g_ScreenJoystick->HandleTouch( L_MOTION_START, LVector2(), false, L_MOTION_MOVE );
			g_ScreenJoystick->HandleTouch( 0, g_MouseState.FPos, g_MouseState.FPressed, L_MOTION_MOVE );
			g_ScreenJoystick->HandleTouch( L_MOTION_END, LVector2(), false, L_MOTION_MOVE );
		}
	}

	virtual bool OnTouch( int X, int Y, bool Touch ) override
	{
		auto Flag = Touch ? L_MOTION_UP : L_MOTION_DOWN;

		g_ScreenJoystick->HandleTouch( L_MOTION_START, g_MouseState.FPos, false, Flag );
		g_ScreenJoystick->HandleTouch( 0, g_MouseState.FPos, false, Flag );
		g_ScreenJoystick->HandleTouch( L_MOTION_END, vec2(), false, Flag );

		return true;
	}

	virtual bool OnKey( int Key, bool KeyState ) override
	{
		g_Game->OnKey( Key, KeyState );
		return clAsteroidsPage::OnKey( Key, KeyState );
	}

	virtual void Draw( const clPtr<iCanvas>& C ) override
	{
		clGUIPage::Draw( C );

		g_Game->GenerateTicks();

		g_Game->Render();

		// 3. Overlays
		LGL3->glClear( GL_DEPTH_BUFFER_BIT );
		LGL3->glDisable( GL_DEPTH_TEST );
		g_Canvas->SetColor( 255, 255, 255 );

		g_Canvas->TextureRect( 0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_JoystickImage->GetWidth(), g_JoystickImage->GetHeight(), g_JoystickTexture );
	}

	virtual void Update( double DeltaSeconds ) override
	{
//		FGame->Update(DeltaSeconds);
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
//			LOGI("Exiting");
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
