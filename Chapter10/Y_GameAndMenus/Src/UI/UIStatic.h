#include "UI/UIView.h"
#include "VecMath.h"

class clBitmap;

class clUIStatic: public clUIView
{
public:
	clUIStatic()
		: m_TextureHandle( -1 )
		, m_BackgroundColor( 255, 255, 255, 255 )
	{}

	virtual void SetBackgroundColor( const ivec4& C ) { m_BackgroundColor = C;};
	virtual void SetBitmap( const clPtr<clBitmap>& Pixels );

protected:
	virtual void PreDrawView( const clPtr<iCanvas>& C ) override;

private:
	clPtr<clBitmap> m_Bitmap;
	int m_TextureHandle;
	ivec4 m_BackgroundColor;
};
