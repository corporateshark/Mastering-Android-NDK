#pragma once

#include "UI/UIView.h"
#include "VecMath.h"
#include "SDLWindow.h"

class clGUI;

/// GUI page - similar to GUI window but always occupies the whole screen and only one page can be active at a time
class clGUIPage: public clUIView
{
public:
	clGUIPage(): FFallbackPage( NULL ) {}
	virtual ~clGUIPage() {}

	virtual void SetActive();

	virtual void OnActivation() {}
	virtual void OnDeactivation() {}

	virtual void OnMove( int X, int Y ) {}

	virtual bool IsPointOver( int x, int y ) const override
	{
		return true;
	}

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
	clGUI( const clPtr<clSDLWindow>& HostWindow )
		: FActivePage()
		, FPages()
		, FHostWindow( HostWindow )
	{}
	virtual ~clGUI() {}
	void AddPage( const clPtr<clGUIPage>& P )
	{
		P->FGUI = this;
		FPages.push_back( P );
	}
	void SetActivePage( const clPtr<clGUIPage>& Page )
	{
		FHostWindow->SetRootView( Page );

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
			FActivePage->UpdateScreenPositions();

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

	void OnMove( int X, int Y )
	{
		if ( FActivePage )
		{
			FActivePage->OnMove( X, Y );
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
	clPtr<clSDLWindow> FHostWindow;
};
