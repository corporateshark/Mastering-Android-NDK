#include "GamePages.h"

#include "TextRenderer.h"

clPtr<clGUI> g_GUI;

clPtr<clMenuPage> g_MenuPage;
clPtr<clOptionsPage> g_OptionsPage;
clPtr<clGamePage> g_GamePage;

clPtr<clUIButton> clAsteroidsPage::SetupButton( int X, int Y, int W, int H, const ivec4& Color, const std::string& Title )
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
