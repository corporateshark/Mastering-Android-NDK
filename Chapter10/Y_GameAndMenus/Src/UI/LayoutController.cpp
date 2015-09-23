#include "UI/LayoutController.h"

void clVerticalLayout::LayoutChildViews()
{
	int W = this->GetWidth();
	int H = this->GetHeight();

	int NumFilledViews = 0;

	// 1. Measure
	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( ( Mode & eFillMode_Vertical ) == 0 )
		{
			// view has fixed height, reduce the amount of vertical free space
			H -= i->GetHeight();
		}
		else
		{
			NumFilledViews++;
		}
	}

	// 2. Apply
	int NextPosY = 0;

	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( Mode & eFillMode_Vertical )
		{
			i->SetHeight( H / NumFilledViews );
		}

		if ( Mode & eFillMode_Horizontal )
		{
			i->SetWidth( W - i->GetX() );
		}

		i->SetPosition( i->GetX(), NextPosY );
		NextPosY += i->GetHeight();
	}

	// 3. Process children
	for ( auto& i : this->GetChildViews() )
	{
		switch ( i->GetAlignmentH() )
		{
			case eAlignH_Left:
				i->SetPosition( 0, i->GetY() );
				break;

			case eAlignH_Right:
				i->SetPosition( W - i->GetWidth(), i->GetY() );
				break;

			case eAlignH_Center:
				i->SetPosition( ( W - i->GetWidth() ) / 2, i->GetY() );
				break;

			case eAlignH_DontCare:
				break;
		}

		i->LayoutChildViews();
	}
}

void clHorizontalLayout::LayoutChildViews()
{
	int W = this->GetWidth();
	int H = this->GetHeight();

	int NumFilledViews = 0;

	// 1. Measure
	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( ( Mode & eFillMode_Horizontal ) == 0 )
		{
			// view has fixed width, reduce the amount of horizontal free space
			W -= i->GetWidth();
		}
		else
		{
			NumFilledViews++;
		}
	}

	// 2. Apply
	int NextPosX = 0;

	for ( auto& i : this->GetChildViews() )
	{
		int Mode = i->GetFillMode();

		if ( Mode & eFillMode_Vertical )
		{
			i->SetHeight( H - i->GetY() );
		}

		if ( Mode & eFillMode_Horizontal )
		{
			i->SetWidth( W / NumFilledViews );
		}

		i->SetPosition( NextPosX, i->GetY() );
		NextPosX += i->GetWidth();
	}

	// 3. Process children
	for ( auto& i : this->GetChildViews() )
	{
		i->LayoutChildViews();
	}
}
