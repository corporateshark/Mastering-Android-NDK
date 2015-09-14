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

#include <memory.h>

#include <fstream>
#include <iterator>
#include <algorithm>

#include "FI_Utils.h"

#ifndef _FI_SaveLoadFlags_h_
#define _FI_SaveLoadFlags_h_

#if defined( ANDROID ) || defined( OS_MACOSX )
// Android uses statically linked FreeImage
#define FREEIMAGE_LIB
//	#include "ExtLibs/FreeImage.h"
#endif

//#else // OS_ANDROID


// extracted from FreeImage.h
#if defined(FREEIMAGE_LIB)
#define DLL_API
#define DLL_CALLCONV
#else
#if defined(_WIN32) || defined(__WIN32__)
#define DLL_CALLCONV __stdcall
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the FREEIMAGE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef FREEIMAGE_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // FREEIMAGE_EXPORTS
#else
// try the gcc visibility support (see http://gcc.gnu.org/wiki/Visibility)
#if defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#ifndef GCC_HASCLASSVISIBILITY
#define GCC_HASCLASSVISIBILITY
#endif
#endif // __GNUC__
#define DLL_CALLCONV
#if defined(GCC_HASCLASSVISIBILITY)
#define DLL_API __attribute__ ((visibility("default")))
#else
#define DLL_API
#endif
#endif // WIN32 / !WIN32
#endif // FREEIMAGE_LIB

#define BMP_DEFAULT         0
#define BMP_SAVE_RLE        1
#define CUT_DEFAULT         0
#define DDS_DEFAULT        0
#define EXR_DEFAULT        0     // save data as half with piz-based wavelet compression
#define EXR_FLOAT       0x0001   // save data as float instead of as half (not recommended)
#define EXR_NONE        0x0002   // save with no compression
#define EXR_ZIP            0x0004   // save with zlib compression, in blocks of 16 scan lines
#define EXR_PIZ            0x0008   // save with piz-based wavelet compression
#define EXR_PXR24       0x0010   // save with lossy 24-bit float compression
#define EXR_B44            0x0020   // save with lossy 44% float compression - goes to 22% when combined with EXR_LC
#define EXR_LC          0x0040   // save images with one luminance and two chroma channels, rather than as RGB (lossy compression)
#define FAXG3_DEFAULT      0
#define GIF_DEFAULT        0
#define GIF_LOAD256        1     // Load the image as a 256 color image with ununsed palette entries, if it's 16 or 2 color
#define GIF_PLAYBACK    2     // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when loading
#define HDR_DEFAULT        0
#define ICO_DEFAULT         0
#define ICO_MAKEALPHA      1     // convert to 32bpp and create an alpha channel from the AND-mask when loading
#define IFF_DEFAULT         0
#define J2K_DEFAULT        0     // save with a 16:1 rate
#define JP2_DEFAULT        0     // save with a 16:1 rate
#define JPEG_DEFAULT        0    // loading (see JPEG_FAST); saving (see JPEG_QUALITYGOOD|JPEG_SUBSAMPLING_420)
#define JPEG_FAST           0x0001  // load the file as fast as possible, sacrificing some quality
#define JPEG_ACCURATE       0x0002  // load the file with the best quality, sacrificing some speed
#define JPEG_CMYK       0x0004   // load separated CMYK "as is" (use | to combine with other load flags)
#define JPEG_EXIFROTATE    0x0008   // load and rotate according to Exif 'Orientation' tag if available
#define JPEG_QUALITYSUPERB  0x80 // save with superb quality (100:1)
#define JPEG_QUALITYGOOD    0x0100  // save with good quality (75:1)
#define JPEG_QUALITYNORMAL  0x0200  // save with normal quality (50:1)
#define JPEG_QUALITYAVERAGE 0x0400  // save with average quality (25:1)
#define JPEG_QUALITYBAD     0x0800  // save with bad quality (10:1)
#define JPEG_PROGRESSIVE   0x2000   // save as a progressive-JPEG (use | to combine with other save flags)
#define JPEG_SUBSAMPLING_411 0x1000    // save with high 4x1 chroma subsampling (4:1:1) 
#define JPEG_SUBSAMPLING_420 0x4000    // save with medium 2x2 medium chroma subsampling (4:2:0) - default value
#define JPEG_SUBSAMPLING_422 0x8000    // save with low 2x1 chroma subsampling (4:2:2) 
#define JPEG_SUBSAMPLING_444 0x10000   // save with no chroma subsampling (4:4:4)
#define JPEG_OPTIMIZE      0x20000     // on saving, compute optimal Huffman coding tables (can reduce a few percent of file size)
#define JPEG_BASELINE      0x40000     // save basic JPEG, without metadata or any markers
#define KOALA_DEFAULT       0
#define LBM_DEFAULT         0
#define MNG_DEFAULT         0
#define PCD_DEFAULT         0
#define PCD_BASE            1    // load the bitmap sized 768 x 512
#define PCD_BASEDIV4        2    // load the bitmap sized 384 x 256
#define PCD_BASEDIV16       3    // load the bitmap sized 192 x 128
#define PCX_DEFAULT         0
#define PFM_DEFAULT         0
#define PICT_DEFAULT        0
#define PNG_DEFAULT         0
#define PNG_IGNOREGAMMA    1     // loading: avoid gamma correction
#define PNG_Z_BEST_SPEED         0x0001   // save using ZLib level 1 compression flag (default value is 6)
#define PNG_Z_DEFAULT_COMPRESSION   0x0006   // save using ZLib level 6 compression flag (default recommended value)
#define PNG_Z_BEST_COMPRESSION      0x0009   // save using ZLib level 9 compression flag (default value is 6)
#define PNG_Z_NO_COMPRESSION     0x0100   // save without ZLib compression
#define PNG_INTERLACED           0x0200   // save using Adam7 interlacing (use | to combine with other save flags)
#define PNM_DEFAULT         0
#define PNM_SAVE_RAW        0       // If set the writer saves in RAW format (i.e. P4, P5 or P6)
#define PNM_SAVE_ASCII      1       // If set the writer saves in ASCII format (i.e. P1, P2 or P3)
#define PSD_DEFAULT         0
#define PSD_CMYK        1     // reads tags for separated CMYK (default is conversion to RGB)
#define PSD_LAB            2     // reads tags for CIELab (default is conversion to RGB)
#define RAS_DEFAULT         0
#define RAW_DEFAULT         0    // load the file as linear RGB 48-bit
#define RAW_PREVIEW        1     // try to load the embedded JPEG preview with included Exif Data or default to RGB 24-bit
#define RAW_DISPLAY        2     // load the file as RGB 24-bit
#define RAW_HALFSIZE    4     // output a half-size color image
#define SGI_DEFAULT        0
#define TARGA_DEFAULT       0
#define TARGA_LOAD_RGB888   1       // If set the loader converts RGB555 and ARGB8888 -> RGB888.
#define TARGA_SAVE_RLE     2     // If set, the writer saves with RLE compression
#define TIFF_DEFAULT        0
#define TIFF_CMYK       0x0001   // reads/stores tags for separated CMYK (use | to combine with compression flags)
#define TIFF_PACKBITS       0x0100  // save using PACKBITS compression
#define TIFF_DEFLATE        0x0200  // save using DEFLATE compression (a.k.a. ZLIB compression)
#define TIFF_ADOBE_DEFLATE  0x0400  // save using ADOBE DEFLATE compression
#define TIFF_NONE           0x0800  // save without any compression
#define TIFF_CCITTFAX3     0x1000  // save using CCITT Group 3 fax encoding
#define TIFF_CCITTFAX4     0x2000  // save using CCITT Group 4 fax encoding
#define TIFF_LZW        0x4000   // save using LZW compression
#define TIFF_JPEG       0x8000   // save using JPEG compression
#define TIFF_LOGLUV        0x10000  // save using LogLuv compression
#define WBMP_DEFAULT        0
#define XBM_DEFAULT        0
#define XPM_DEFAULT        0


enum FREE_IMAGE_FORMAT
{
	FIF_UNKNOWN = -1,
	FIF_BMP     = 0,
	FIF_ICO     = 1,
	FIF_JPEG = 2,
	FIF_JNG     = 3,
	FIF_KOALA   = 4,
	FIF_LBM     = 5,
	FIF_IFF = FIF_LBM,
	FIF_MNG     = 6,
	FIF_PBM     = 7,
	FIF_PBMRAW  = 8,
	FIF_PCD     = 9,
	FIF_PCX     = 10,
	FIF_PGM     = 11,
	FIF_PGMRAW  = 12,
	FIF_PNG     = 13,
	FIF_PPM     = 14,
	FIF_PPMRAW  = 15,
	FIF_RAS     = 16,
	FIF_TARGA   = 17,
	FIF_TIFF = 18,
	FIF_WBMP = 19,
	FIF_PSD     = 20,
	FIF_CUT     = 21,
	FIF_XBM     = 22,
	FIF_XPM     = 23,
	FIF_DDS     = 24,
	FIF_GIF     = 25,
	FIF_HDR     = 26,
	FIF_FAXG3   = 27,
	FIF_SGI     = 28,
	FIF_EXR     = 29,
	FIF_J2K     = 30,
	FIF_JP2     = 31
};

enum FREE_IMAGE_FILTER
{
	FILTER_BOX       = 0,   // Box, pulse, Fourier window, 1st order (constant) b-spline
	FILTER_BICUBIC   = 1,   // Mitchell & Netravali's two-param cubic filter
	FILTER_BILINEAR   = 2,  // Bilinear filter
	FILTER_BSPLINE   = 3,   // 4th order (cubic) b-spline
	FILTER_CATMULLROM = 4,  // Catmull-Rom spline, Overhauser spline
	FILTER_LANCZOS3     = 5 // Lanczos3 filter
};

typedef struct
{
	void* data;
} FIBITMAP;

typedef struct
{
	void* data;
} FIMEMORY;

// #endif // OS_ANDROID

typedef FIMEMORY* ( DLL_CALLCONV* PFNFreeImage_OpenMemory ) ( void*, unsigned int );
typedef void      ( DLL_CALLCONV* PFNFreeImage_CloseMemory ) ( FIMEMORY* );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_LoadFromMemory ) ( FREE_IMAGE_FORMAT, FIMEMORY*, int );
typedef bool      ( DLL_CALLCONV* PFNFreeImage_AcquireMemory ) ( FIMEMORY* stream, unsigned char** data, unsigned int* size_in_bytes );
typedef FREE_IMAGE_FORMAT ( DLL_CALLCONV* PFNFreeImage_GetFileTypeFromMemory ) ( FIMEMORY*, int );
typedef unsigned  ( DLL_CALLCONV* PFNFreeImage_GetBPP ) ( FIBITMAP* );
typedef unsigned  ( DLL_CALLCONV* PFNFreeImage_GetWidth )( FIBITMAP* );
typedef unsigned  ( DLL_CALLCONV* PFNFreeImage_GetHeight )( FIBITMAP* );
typedef unsigned  ( DLL_CALLCONV* PFNFreeImage_GetPitch )( FIBITMAP* );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_ConvertToRGBF )( FIBITMAP* );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_ConvertTo24Bits )( FIBITMAP* );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_ConvertTo32Bits )( FIBITMAP* );
typedef void      ( DLL_CALLCONV* PFNFreeImage_Unload )( FIBITMAP* );
typedef int       ( DLL_CALLCONV* PFNFreeImage_IsTransparent )( FIBITMAP* );
typedef int       ( DLL_CALLCONV* PFNFreeImage_FlipVertical )( FIBITMAP* );
typedef unsigned char*   ( DLL_CALLCONV* PFNFreeImage_GetBits )( FIBITMAP* );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_Allocate )( int, int, int, unsigned, unsigned, unsigned );
typedef bool      ( DLL_CALLCONV* PFNFreeImage_Save )( FREE_IMAGE_FORMAT, FIBITMAP*, const char*, int );
typedef bool      ( DLL_CALLCONV* PFNFreeImage_SaveToMemory )( FREE_IMAGE_FORMAT fif, FIBITMAP*, FIMEMORY*, int );
typedef void      ( DLL_CALLCONV* PFNFreeImage_Initialise )( int );
typedef void      ( DLL_CALLCONV* PFNFreeImage_DeInitialise )( void );

typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_ConvertFromRawBits )( void*, int , int , int , unsigned , unsigned , unsigned , unsigned , int );
typedef void      ( DLL_CALLCONV* PFNFreeImage_ConvertToRawBits )( void*, FIBITMAP*, int , unsigned , unsigned , unsigned , unsigned , int );

typedef unsigned char*   ( DLL_CALLCONV* PFNFreeImage_GetScanLine )( FIBITMAP* dib, int scanline );
typedef FIBITMAP* ( DLL_CALLCONV* PFNFreeImage_Rescale )( FIBITMAP* dib, int dst_width, int dst_height, FREE_IMAGE_FILTER filter );

#endif


#include <string>

//#include "Bitmap.h"

/// Load image from stream into the resource with optional vertical flip
//bool FreeImage_LoadFromStream( const void* Data, unsigned int Size, const clPtr<clBitmap>& OutBitmap, bool DoFlipV );

#if defined( _WIN32 )

#include <windows.h>

#if !defined( _WIN64 )
const std::string FreeImageDLL = "freeimage32.dll";
#else
const std::string FreeImageDLL = "freeimage64.dll";
#endif

HMODULE Lib = NULL;
#else
// Android uses statically linked FreeImage. LV: But this doesn't mean we use some of its headers directly
//	#include "ExtLibs/FreeImage.h"
extern "C"
{
	DLL_API void DLL_CALLCONV FreeImage_Initialise( bool load_local_plugins_only );
	DLL_API void DLL_CALLCONV FreeImage_DeInitialise( void );

// Allocate / Clone / Unload routines ---------------------------------------

	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_Allocate( int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask );
	DLL_API void DLL_CALLCONV FreeImage_Unload( FIBITMAP* dib );

// Load / Save routines -----------------------------------------------------

	DLL_API bool DLL_CALLCONV FreeImage_Save( FREE_IMAGE_FORMAT fif, FIBITMAP* dib, const char* filename, int flags );

// Memory I/O stream routines -----------------------------------------------

	DLL_API FIMEMORY* DLL_CALLCONV FreeImage_OpenMemory( ubyte data, uint32_t size_in_bytes );
	DLL_API bool      DLL_CALLCONV FreeImage_AcquireMemory( FIMEMORY* stream, ubyte** data, uint32_t* size_in_bytes );
	DLL_API void DLL_CALLCONV FreeImage_CloseMemory( FIMEMORY* stream );

	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_LoadFromMemory( FREE_IMAGE_FORMAT fif, FIMEMORY* stream, int flags );
	DLL_API bool DLL_CALLCONV FreeImage_SaveToMemory( FREE_IMAGE_FORMAT fif, FIBITMAP* dib, FIMEMORY* stream, int flags );
	DLL_API FREE_IMAGE_FORMAT DLL_CALLCONV FreeImage_GetFileTypeFromMemory( FIMEMORY* stream, int size );

// Pixel access routines ----------------------------------------------------
	DLL_API ubyte* DLL_CALLCONV FreeImage_GetBits( FIBITMAP* dib );

	DLL_API unsigned DLL_CALLCONV FreeImage_GetPitch( FIBITMAP* dib );

	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertFromRawBits ( void*, int , int , int , unsigned , unsigned , unsigned , unsigned , int );
	DLL_API void      DLL_CALLCONV FreeImage_ConvertToRawBits ( void*, FIBITMAP*, int , unsigned , unsigned , unsigned , unsigned , int );

	DLL_API ubyte*   DLL_CALLCONV FreeImage_GetScanLine( FIBITMAP* dib, int scanline );

	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_Rescale( FIBITMAP* dib, int dst_width, int dst_height, FREE_IMAGE_FILTER filter );

// DIB info routines --------------------------------------------------------

	DLL_API unsigned DLL_CALLCONV FreeImage_GetBPP( FIBITMAP* dib );
	DLL_API unsigned DLL_CALLCONV FreeImage_GetWidth( FIBITMAP* dib );
	DLL_API unsigned DLL_CALLCONV FreeImage_GetHeight( FIBITMAP* dib );

// Smart conversion routines ------------------------------------------------
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertTo24Bits( FIBITMAP* dib );
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertTo32Bits( FIBITMAP* dib );
	DLL_API FIBITMAP* DLL_CALLCONV FreeImage_ConvertToRGBF( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_IsTransparent( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_FlipHorizontal( FIBITMAP* dib );
	DLL_API bool DLL_CALLCONV FreeImage_FlipVertical( FIBITMAP* dib );
}

#endif // OS_ANDROID

PFNFreeImage_Initialise            FI_Initialise = NULL;
PFNFreeImage_DeInitialise          FI_DeInitialise = NULL;
PFNFreeImage_OpenMemory            FI_OpenMemory = NULL;
PFNFreeImage_CloseMemory           FI_CloseMemory = NULL;
PFNFreeImage_LoadFromMemory        FI_LoadFromMemory = NULL;
PFNFreeImage_AcquireMemory         FI_AcquireMemory = NULL;
PFNFreeImage_GetFileTypeFromMemory FI_GetFileTypeFromMemory = NULL;
PFNFreeImage_GetBPP                FI_GetBPP = NULL;
PFNFreeImage_GetWidth              FI_GetWidth = NULL;
PFNFreeImage_GetHeight             FI_GetHeight = NULL;
PFNFreeImage_ConvertToRGBF         FI_ConvertToRGBF = NULL;
PFNFreeImage_ConvertTo24Bits       FI_ConvertTo24Bits = NULL;
PFNFreeImage_ConvertTo32Bits       FI_ConvertTo32Bits = NULL;
PFNFreeImage_Unload                FI_Unload = NULL;
PFNFreeImage_IsTransparent         FI_IsTransparent = NULL;
PFNFreeImage_FlipVertical          FI_FlipVertical = NULL;
PFNFreeImage_GetBits               FI_GetBits = NULL;
PFNFreeImage_Allocate              FI_Allocate = NULL;
PFNFreeImage_Save                  FI_Save = NULL;
PFNFreeImage_SaveToMemory          FI_SaveToMemory = NULL;
PFNFreeImage_GetPitch              FI_GetPitch = NULL;
PFNFreeImage_ConvertFromRawBits    FI_ConvertFromRawBits = NULL;
PFNFreeImage_ConvertToRawBits      FI_ConvertToRawBits = NULL;
PFNFreeImage_GetScanLine           FI_GetScanLine = NULL;
PFNFreeImage_Rescale               FI_Rescale = NULL;

void UnloadFreeImage()
{
#if defined( _WIN32 )
	FreeLibrary( Lib );

	FI_OpenMemory = NULL;
	Lib = NULL;
#endif // OS_ANDROID
}

bool LoadFreeImage()
{
#if !defined( _WIN32 )
	FI_Initialise = ( PFNFreeImage_Initialise )&FreeImage_Initialise;
	FI_DeInitialise = ( PFNFreeImage_DeInitialise )&FreeImage_DeInitialise;
	FI_OpenMemory = ( PFNFreeImage_OpenMemory )&FreeImage_OpenMemory;
	FI_CloseMemory = ( PFNFreeImage_CloseMemory )&FreeImage_CloseMemory;
	FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )&FreeImage_LoadFromMemory;
	FI_AcquireMemory = ( PFNFreeImage_AcquireMemory )&FreeImage_AcquireMemory;
	FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )&FreeImage_GetFileTypeFromMemory;
	FI_GetBPP = ( PFNFreeImage_GetBPP )&FreeImage_GetBPP;
	FI_GetWidth = ( PFNFreeImage_GetWidth )&FreeImage_GetWidth;
	FI_GetHeight = ( PFNFreeImage_GetHeight )&FreeImage_GetHeight;
	FI_GetPitch = ( PFNFreeImage_GetPitch )&FreeImage_GetPitch;
	FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )&FreeImage_ConvertToRGBF;
	FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )&FreeImage_ConvertTo24Bits;
	FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )&FreeImage_ConvertTo32Bits;
	FI_Unload = ( PFNFreeImage_Unload )&FreeImage_Unload;
	FI_IsTransparent = ( PFNFreeImage_IsTransparent )&FreeImage_IsTransparent;
	FI_FlipVertical = ( PFNFreeImage_FlipVertical )&FreeImage_FlipVertical;
	FI_GetBits = ( PFNFreeImage_GetBits )&FreeImage_GetBits;
	FI_Allocate = ( PFNFreeImage_Allocate )&FreeImage_Allocate;
	FI_Save = ( PFNFreeImage_Save )&FreeImage_Save;
	FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )&FreeImage_SaveToMemory;

	FI_ConvertFromRawBits = ( PFNFreeImage_ConvertFromRawBits )&FreeImage_ConvertFromRawBits;
	FI_ConvertToRawBits = ( PFNFreeImage_ConvertToRawBits )&FreeImage_ConvertToRawBits;

	FI_GetScanLine = ( PFNFreeImage_GetScanLine )&FreeImage_GetScanLine;

	FI_Rescale = ( PFNFreeImage_Rescale )&FreeImage_Rescale;

#else
	Lib = LoadLibrary( FreeImageDLL.c_str() );

	FI_Initialise = ( PFNFreeImage_Initialise )GetProcAddress( Lib, "FreeImage_Initialise" );
	FI_DeInitialise = ( PFNFreeImage_DeInitialise )GetProcAddress( Lib, "FreeImage_DeInitialise" );
	FI_OpenMemory = ( PFNFreeImage_OpenMemory )GetProcAddress( Lib, "FreeImage_OpenMemory" );
	FI_CloseMemory = ( PFNFreeImage_CloseMemory )GetProcAddress( Lib, "FreeImage_CloseMemory" );
	FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )GetProcAddress( Lib, "FreeImage_LoadFromMemory" );
	FI_AcquireMemory = ( PFNFreeImage_AcquireMemory )GetProcAddress( Lib, "FreeImage_AcquireMemory" );
	FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )GetProcAddress( Lib, "FreeImage_GetFileTypeFromMemory" );
	FI_GetBPP = ( PFNFreeImage_GetBPP )GetProcAddress( Lib, "FreeImage_GetBPP" );
	FI_GetWidth = ( PFNFreeImage_GetWidth )GetProcAddress( Lib, "FreeImage_GetWidth" );
	FI_GetHeight = ( PFNFreeImage_GetHeight )GetProcAddress( Lib, "FreeImage_GetHeight" );
	FI_GetPitch = ( PFNFreeImage_GetPitch )GetProcAddress( Lib, "FreeImage_GetPitch" );
	FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )GetProcAddress( Lib, "FreeImage_ConvertToRGBF" );
	FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )GetProcAddress( Lib, "FreeImage_ConvertTo24Bits" );
	FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )GetProcAddress( Lib, "FreeImage_ConvertTo32Bits" );
	FI_Unload = ( PFNFreeImage_Unload )GetProcAddress( Lib, "FreeImage_Unload" );
	FI_IsTransparent = ( PFNFreeImage_IsTransparent )GetProcAddress( Lib, "FreeImage_IsTransparent" );
	FI_FlipVertical = ( PFNFreeImage_FlipVertical )GetProcAddress( Lib, "FreeImage_FlipVertical" );
	FI_GetBits = ( PFNFreeImage_GetBits )GetProcAddress( Lib, "FreeImage_GetBits" );
	FI_Allocate = ( PFNFreeImage_Allocate )GetProcAddress( Lib, "FreeImage_Allocate" );
	FI_Save = ( PFNFreeImage_Save )GetProcAddress( Lib, "FreeImage_Save" );
	FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )GetProcAddress( Lib, "FreeImage_SaveToMemory" );

	FI_ConvertFromRawBits = ( PFNFreeImage_ConvertFromRawBits )GetProcAddress( Lib, "FreeImage_ConvertFromRawBits" );
	FI_ConvertToRawBits = ( PFNFreeImage_ConvertToRawBits )GetProcAddress( Lib, "FreeImage_ConvertToRawBits" );

	FI_GetScanLine = ( PFNFreeImage_GetScanLine )GetProcAddress( Lib, "FreeImage_GetScanLine" );
	FI_Rescale = ( PFNFreeImage_Rescale )GetProcAddress( Lib, "FreeImage_Rescale" );

	if ( !FI_Initialise )
	{
		// some Win32 / C++ name mangling black magic
		FI_Initialise = ( PFNFreeImage_Initialise )GetProcAddress( Lib, "_FreeImage_Initialise@4" );
		FI_DeInitialise = ( PFNFreeImage_DeInitialise )GetProcAddress( Lib, "_FreeImage_DeInitialise@0" );
		FI_OpenMemory = ( PFNFreeImage_OpenMemory )GetProcAddress( Lib, "_FreeImage_OpenMemory@8" );
		FI_CloseMemory = ( PFNFreeImage_CloseMemory )GetProcAddress( Lib, "_FreeImage_CloseMemory@4" );
		FI_LoadFromMemory = ( PFNFreeImage_LoadFromMemory )GetProcAddress( Lib, "_FreeImage_LoadFromMemory@12" );
		FI_AcquireMemory = ( PFNFreeImage_AcquireMemory )GetProcAddress( Lib, "_FreeImage_AcquireMemory@12" );
		FI_GetFileTypeFromMemory = ( PFNFreeImage_GetFileTypeFromMemory )GetProcAddress( Lib, "_FreeImage_GetFileTypeFromMemory@8" );
		FI_GetBPP = ( PFNFreeImage_GetBPP )GetProcAddress( Lib, "_FreeImage_GetBPP@4" );
		FI_GetWidth = ( PFNFreeImage_GetWidth )GetProcAddress( Lib, "_FreeImage_GetWidth@4" );
		FI_GetHeight = ( PFNFreeImage_GetHeight )GetProcAddress( Lib, "_FreeImage_GetHeight@4" );
		FI_GetPitch = ( PFNFreeImage_GetPitch )GetProcAddress( Lib, "_FreeImage_GetPitch@4" );
		FI_ConvertToRGBF = ( PFNFreeImage_ConvertToRGBF )GetProcAddress( Lib, "_FreeImage_ConvertToRGBF@4" );
		FI_ConvertTo24Bits = ( PFNFreeImage_ConvertTo24Bits )GetProcAddress( Lib, "_FreeImage_ConvertTo24Bits@4" );
		FI_ConvertTo32Bits = ( PFNFreeImage_ConvertTo32Bits )GetProcAddress( Lib, "_FreeImage_ConvertTo32Bits@4" );
		FI_Unload = ( PFNFreeImage_Unload )GetProcAddress( Lib, "_FreeImage_Unload@4" );
		FI_IsTransparent = ( PFNFreeImage_IsTransparent )GetProcAddress( Lib, "_FreeImage_IsTransparent@4" );
		FI_FlipVertical = ( PFNFreeImage_FlipVertical )GetProcAddress( Lib, "_FreeImage_FlipVertical@4" );
		FI_GetBits = ( PFNFreeImage_GetBits )GetProcAddress( Lib, "_FreeImage_GetBits@4" );
		FI_Allocate = ( PFNFreeImage_Allocate )GetProcAddress( Lib, "_FreeImage_Allocate@24" );
		FI_Save = ( PFNFreeImage_Save )GetProcAddress( Lib, "_FreeImage_Save@16" );
		FI_SaveToMemory = ( PFNFreeImage_SaveToMemory )GetProcAddress( Lib, "_FreeImage_SaveToMemory@16" );
		FI_ConvertFromRawBits = ( PFNFreeImage_ConvertFromRawBits )GetProcAddress( Lib, "_FreeImage_ConvertFromRawBits@36" );
		FI_ConvertToRawBits = ( PFNFreeImage_ConvertToRawBits )GetProcAddress( Lib, "_FreeImage_ConvertToRawBits@32" );

		FI_GetScanLine = ( PFNFreeImage_GetScanLine )GetProcAddress( Lib, "_FreeImage_GetScanLine@8" );
		FI_Rescale = ( PFNFreeImage_Rescale )GetProcAddress( Lib, "_FreeImage_Rescale@16" );
	}

#endif // OS_ANDROID

	FI_Initialise( false );

	return true;
}

// No floating-point formats, to be added later
bool FreeImage_LoadFromStream( void* Data, unsigned int Size, std::vector<ubyte>& OutData, int& W, int& H, int& BitsPP, bool DoFlipV )
{
	FIMEMORY* Mem = FI_OpenMemory( ( unsigned char* )Data, static_cast<unsigned int>( Size ) );

	FREE_IMAGE_FORMAT FIF = FI_GetFileTypeFromMemory( Mem, 0 );

	FIBITMAP* Bitmap = FI_LoadFromMemory( FIF, Mem, 0 );
	FIBITMAP* ConvBitmap;

	FI_CloseMemory( Mem );

	bool FloatFormat = FI_GetBPP( Bitmap ) > 32;

	if ( FloatFormat )
	{
		// process high-dynamic range images
		ConvBitmap = FI_ConvertToRGBF( Bitmap );
	}
	else
	{
		ConvBitmap = FI_IsTransparent( Bitmap ) ? FI_ConvertTo32Bits( Bitmap ) : FI_ConvertTo24Bits( Bitmap );
	}

	FI_Unload( Bitmap );

	Bitmap = ConvBitmap;

	if ( DoFlipV ) { FI_FlipVertical( Bitmap ); }

	int Width  = FI_GetWidth( Bitmap );
	int Height = FI_GetHeight( Bitmap );

	W = Width;
	H = Height;
	BitsPP = FI_GetBPP( Bitmap );

	OutData.resize( W * H * ( BitsPP / 8 ) );

	FI_ConvertToRawBits( &OutData[0], Bitmap, W * BitsPP / 8, BitsPP, 0, 1, 2, false );

	FI_Unload( Bitmap );

	return true;
}

static void FileExtToFreeImageFormats( std::string Ext, int& OutSubFormat, FREE_IMAGE_FORMAT& OutFormat )
{
	OutSubFormat = TIFF_LZW;
	OutFormat = FIF_TIFF;

	std::for_each( Ext.begin(), Ext.end(), []( char& in ) { in = ::toupper( in ); } );

	if ( Ext == ".PNG" )
	{
		OutFormat = FIF_PNG;
		OutSubFormat = PNG_DEFAULT;
	}
	else if ( Ext == ".BMP" )
	{
		OutFormat = FIF_BMP;
		OutSubFormat = BMP_DEFAULT;
	}
	else if ( Ext == ".JPG" )
	{
		OutFormat = FIF_JPEG;
		OutSubFormat = JPEG_QUALITYSUPERB | JPEG_BASELINE | JPEG_PROGRESSIVE | JPEG_OPTIMIZE;
	}
	else if ( Ext == ".EXR" )
	{
		OutFormat = FIF_EXR;
		OutSubFormat = EXR_FLOAT;
	}
}

bool FreeImage_SaveToMemory( const std::string& Ext, ubyte* RawBGRImage, int Width, int Height, int BitsPP, std::vector<ubyte>& OutData )
{
	FIBITMAP* Bitmap = FI_ConvertFromRawBits( RawBGRImage, Width, Height, Width * BitsPP / 8, BitsPP, 0, 1, 2, true );

	int OutSubFormat;
	FREE_IMAGE_FORMAT OutFormat;

	FileExtToFreeImageFormats( Ext, OutSubFormat, OutFormat );

	// use FI's mem manager
	FIMEMORY* Mem = FI_OpenMemory( 0, 0 );

	if ( !FI_SaveToMemory( OutFormat, Bitmap, Mem, OutSubFormat ) )
	{
		return false;
	}

	ubyte* Data = NULL;
	uint32_t Size = 0;

	// get direct access to the FI's memory block
	FI_AcquireMemory( Mem, &Data, &Size );

	OutData.resize( Size );
	memcpy( &OutData[0], Data, Size );

	FI_CloseMemory( Mem );
	FI_Unload( Bitmap );

	return true;
}

std::string ExtractExtension( const std::string& FileName )
{
	size_t pos = FileName.find_last_of( '.' );

	return ( pos == std::string::npos ) ? FileName : FileName.substr( pos );
}

bool FreeImage_SaveBitmapToFile( const std::string& FileName, ubyte* ImageData, int W, int H, int BitsPP )
{
	std::string Ext = ExtractExtension( FileName );

	std::vector<ubyte> OutData;

	if ( !FreeImage_SaveToMemory( Ext, ImageData, W, H, BitsPP, OutData ) )
	{
		return false;
	}

	std::ofstream OutFile( FileName.c_str(), std::ios::out | std::ofstream::binary );
	std::copy( OutData.begin(), OutData.end(), std::ostreambuf_iterator<char>( OutFile ) );

	return true;
}

bool FreeImage_LoadBitmapFromFile( const std::string& FileName, std::vector<ubyte>& OutData, int& W, int& H, int& BitsPP )
{
	std::ifstream InFile( FileName.c_str(), std::ios::in | std::ifstream::binary );
	std::vector<char> data( ( std::istreambuf_iterator<char>( InFile ) ), std::istreambuf_iterator<char>() );

	return FreeImage_LoadFromStream( ( ubyte* )&data[0], ( int )data.size(), OutData, W, H, BitsPP, true );
}

#if defined( ANDROID )
extern "C"
{
	// patch for libc
	int __srget( FILE* fp )
	{
		return EOF;
	}
}
#endif
