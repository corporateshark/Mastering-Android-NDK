/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov'in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>

#include "FI_Utils.h"
#include "FT_Utils.h"

int GetPixel( const std::vector<unsigned char>& Bitmap, int W, int H, int x, int y )
{
	if ( y >= H || x >= W || y < 0 || x < 0 ) { return 0; }

	int offset = ( y * W + x ) * 3;
	return ( Bitmap[offset + 0] << 16 ) + ( Bitmap[offset + 1] << 8 ) + Bitmap[offset + 2];
}

void PutPixel( std::vector<unsigned char>& Bitmap, int W, int H, int x, int y, int Color )
{
	if ( y < 0 || x < 0 || y > H - 1 || x > W - 1 ) { return; }

	int Ofs = y * W + x;
	Bitmap[Ofs * 3 + 0] = Color & 0xFF;
	Bitmap[Ofs * 3 + 1] = ( Color >> 8 ) & 0xFF;
	Bitmap[Ofs * 3 + 2] = ( Color >> 16 ) & 0xFF;
}

/// Horizontal size of the character
const int CharW = 32;
const int CharH = 64;

/// Horizontal size of the character slot
const int SlotW = CharW * 2;
const int SlotH = CharH;

const int FontW = 16 * SlotW;
const int FontH = 16 * SlotH;

inline int Greyscale( unsigned char c )
{
	return ( (255-c) << 16 ) + ( (255-c) << 8 ) + (255-c);
}

std::vector<unsigned char> g_FontBitmap;
std::vector<unsigned char> g_OutBitmap;

int W = 2048;
int H = 2048;

void RenderChar( char c, int x, int y )
{
	int u = ( c % 16 ) * SlotW;
	int v = ( c / 16 ) * SlotH;

	for ( int y1 = 0 ; y1 < CharH ; y1++ )
		for ( int x1 = 0 ; x1 <= CharW ; x1++ )
			PutPixel( g_OutBitmap, W, H, x + x1, y + y1, GetPixel( g_FontBitmap, FontW, FontH, x1 + u + CharW, y1 + v) );
}

void RenderStr( const std::string& Str, int x, int y )
{
	for ( auto c: Str )
	{
		RenderChar( c, x, y );
		x += CharW;
	}
}

void TestFontRendering( const std::vector<char>& Data )
{
	LoadFreeImage();
	LoadFreeType();

	FT_Library Library;
	FT_Init_FreeTypePTR( &Library ); /* initialize library */

	FT_Face Face;
	FT_New_Memory_FacePTR( Library, (const FT_Byte*)Data.data(), (int)Data.size(), 0, &Face );

	/// char size equal to 100dpi
	FT_Set_Char_SizePTR( Face, CharW * 64, 0, 100, 0 );

	g_FontBitmap.resize( FontW * FontH * 3 );
	std::fill( std::begin(g_FontBitmap), std::end(g_FontBitmap), 0xFF );

	for ( int n = 0; n < 256; n++ )
	{
		/* load glyph image into the slot (erase previous one) */
		if ( FT_Load_CharPTR( Face, n , FT_LOAD_RENDER ) ) { continue; }

		FT_GlyphSlot Slot = Face->glyph;
		FT_Bitmap    Bitmap = Slot->bitmap;

		int x = ( n % 16 ) * SlotW + SlotW / 2 + Slot->bitmap_left;
		int y = ( n / 16 ) * SlotH - Slot->bitmap_top + CharH - CharH / 4;

		for ( auto i = 0 ; i < Bitmap.width; i++ )
			for ( auto j = 0 ; j < Bitmap.rows; j++ )
				PutPixel( g_FontBitmap, FontW, FontH, i + x, j + y, Greyscale( Bitmap.buffer[j * Bitmap.width + i] ) );
	}

	FreeImage_SaveBitmapToFile( "test_font.png", g_FontBitmap.data(), FontW, FontH, 24 );

	FT_Done_FacePTR    ( Face );
	FT_Done_FreeTypePTR( Library );

	std::string Str = "Test string";

	W = Str.length() * CharW;
	H = CharH;

	g_OutBitmap.resize( W * H * 3 );
	std::fill( std::begin(g_OutBitmap), std::end(g_OutBitmap), 0xFF );

	RenderStr( Str, 0, 0 );

	FreeImage_SaveBitmapToFile( "test_str.png", g_OutBitmap.data(), W, H, 24 );
}

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		std::cout << "Combined <font.ttf>" << std::endl;
		return 255;
	}

	std::ifstream InFile( argv[1], std::ios::in | std::ifstream::binary );

	TestFontRendering( std::vector<char>( (std::istreambuf_iterator<char>(InFile)), std::istreambuf_iterator<char>() ) );

	return 0;
}
