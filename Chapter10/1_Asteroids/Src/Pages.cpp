#include "Pages.h"

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
