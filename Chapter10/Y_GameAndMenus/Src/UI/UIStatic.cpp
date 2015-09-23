#include "UI/UIStatic.h"
#include "Rendering/Canvas.h"
#include "Bitmap.h"

void clUIStatic::PreDrawView( const clPtr<iCanvas>& C )
{
	if ( m_Bitmap )
	{
		if ( m_TextureHandle == -1 ) { m_TextureHandle = C->CreateTexture( m_Bitmap ); }

		C->TextureRect(
		   m_ScreenX, m_ScreenY, m_Width, m_Height,
		   0, 0, m_Bitmap->GetWidth(), m_Bitmap->GetHeight(),
		   m_TextureHandle
		);
	}
	else
	{
		C->SetColor( m_BackgroundColor );
		C->Rect( m_ScreenX, m_ScreenY, m_Width, m_Height, true );
	}

	clUIView::PreDrawView( C );
}

void clUIStatic::SetBitmap( const clPtr<clBitmap>& Pixels )
{
	m_TextureHandle = -1;
	m_Bitmap = Pixels;
}
