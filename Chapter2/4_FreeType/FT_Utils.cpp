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

#include "FT_Utils.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_SIZES_H

#ifdef _WIN32
#include <windows.h>
#endif

FT_Set_Char_Size_func FT_Set_Char_SizePTR;

FT_Load_Char_func FT_Load_CharPTR;
FT_Done_Face_func FT_Done_FacePTR;

FT_Init_FreeType_func FT_Init_FreeTypePTR;

FT_Activate_Size_func FT_Activate_SizePTR;
FT_Done_FreeType_func FT_Done_FreeTypePTR;

FT_New_Face_func FT_New_FacePTR;
FT_New_Memory_Face_func FT_New_Memory_FacePTR;

FT_Glyph_Get_CBox_func FT_Glyph_Get_CBoxPTR;
FT_Get_Kerning_func FT_Get_KerningPTR;

FTC_Manager_New_func FTC_Manager_NewPTR;
FTC_Manager_Done_func FTC_Manager_DonePTR;

FTC_ImageCache_New_func FTC_ImageCache_NewPTR;
FTC_ImageCache_Lookup_func FTC_ImageCache_LookupPTR;

FTC_CMapCache_New_func FTC_CMapCache_NewPTR;
FTC_CMapCache_Lookup_func FTC_CMapCache_LookupPTR;

FTC_Node_Unref_func FTC_Node_UnrefPTR;

FTC_Manager_LookupFace_func FTC_Manager_LookupFacePTR;
FTC_Manager_LookupSize_func FTC_Manager_LookupSizePTR;

HMODULE hFTDll = NULL;

#undef FT_STATIC_LINK

#if defined(ANDROID) || defined(OS_MACOSX) || defined(OS_LINUX)
#  define FT_STATIC_LINK
#endif

#if defined(FT_STATIC_LINK)
extern "C"
{
	FT_Error FT_Init_FreeType( FT_Library* alibrary );
	FT_Error FT_Done_FreeType( FT_Library  library );
	FT_Error FT_Activate_Size( FT_Size  size );
	FT_Error FT_New_Face( FT_Library library, const char* filepathname, FT_Long face_index, FT_Face* aface );
	FT_Error FT_New_Memory_Face( FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face* aface );
	void     FT_Glyph_Get_CBox( FT_Glyph glyph, FT_UInt bbox_mode, FT_BBox* acbox );
	FT_Error FT_Get_Kerning( FT_Face face, FT_UInt left_glyph, FT_UInt right_glyph, FT_UInt kern_mode, FT_Vector* akerning );
	FT_Error FTC_Manager_New( FT_Library library, FT_UInt max_faces, FT_UInt max_sizes, FT_ULong max_bytes, FTC_Face_Requester requester, FT_Pointer req_data, FTC_Manager* amanager );
	void     FTC_Manager_Done( FTC_Manager manager );
	FT_Error FTC_ImageCache_New( FTC_Manager manager, FTC_ImageCache* acache );
	FT_Error FTC_ImageCache_Lookup( FTC_ImageCache cache, FTC_ImageType type, FT_UInt gindex, FT_Glyph* aglyph, FTC_Node* anode );
	FT_Error FTC_CMapCache_New( FTC_Manager manager, FTC_CMapCache* acache );
	FT_UInt  FTC_CMapCache_Lookup( FTC_CMapCache cache, FTC_FaceID face_id, FT_Int cmap_index, FT_UInt32 char_code );
	void     FTC_Node_Unref( FTC_Node node, FTC_Manager manager );
	FT_Error FTC_Manager_LookupFace( FTC_Manager manager, FTC_FaceID face_id, FT_Face* aface );
	FT_Error FTC_Manager_LookupSize( FTC_Manager manager, FTC_Scaler scaler,  FT_Size* asize );

	FT_Error FT_Set_Char_Size( FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution );
	FT_Error FT_Load_Char( FT_Face face, FT_ULong char_code, FT_Int32 load_flags );
	FT_Error FT_Done_Face( FT_Face  face );
}
#endif // Static link

bool LoadFreeType()
{
	if ( hFTDll != NULL ) { return true; }

#if defined(_WIN32)

#define LOAD_FT_FUNC(Func_) \
   Func_##PTR = (Func_##_func) GetProcAddress( (HMODULE)hFTDll, #Func_ ); \
   if(! Func_##PTR) { /*printf("Cannot load %s function !\n", #Func_ );*/ return false; }

#ifdef _WIN64
	const char* LibName = "libfreetype-6-64.dll";
#else
	const char* LibName = "libfreetype-6-32.dll";
#endif

	*( ( HMODULE* )&hFTDll ) = LoadLibrary( LibName );

	if ( hFTDll == INVALID_HANDLE_VALUE )
	{
		return false;
	}

#else

#  if defined(FT_STATIC_LINK)

#    define LOAD_FT_FUNC(Func_) Func_##PTR = &Func_;

#  else

#    define LOAD_FT_FUNC(Func_) Func_##PTR = (Func_##_func) dlsym( hFTDll, #Func_ ); \
    if(!Func_##PTR) {\
        /* fprintf(stderr, "Error loading FreeType function [%s]\n", #Func_ ); fflush(stderr);*/ \
    return false;\
  }

#  endif

#  if defined(__APPLE__)
#  else

#    if defined(ANDROID)
	// static link, do nothing
#    else
	// .so on Linux and other unixes
	hFTDll = dlopen( "libfreetype.so" /*.so.6*/, RTLD_LAZY );
#    endif

#  endif
#endif

	LOAD_FT_FUNC( FT_Init_FreeType )

	LOAD_FT_FUNC( FT_Activate_Size )
	LOAD_FT_FUNC( FT_Done_FreeType )

	LOAD_FT_FUNC( FT_New_Face )
	LOAD_FT_FUNC( FT_New_Memory_Face )

	LOAD_FT_FUNC( FT_Glyph_Get_CBox )
	LOAD_FT_FUNC( FT_Get_Kerning )

	LOAD_FT_FUNC( FTC_Manager_New )
	LOAD_FT_FUNC( FTC_Manager_Done )

	LOAD_FT_FUNC( FTC_ImageCache_New )
	LOAD_FT_FUNC( FTC_ImageCache_Lookup )

	LOAD_FT_FUNC( FTC_CMapCache_New )
	LOAD_FT_FUNC( FTC_CMapCache_Lookup )

	LOAD_FT_FUNC( FTC_Node_Unref )

	LOAD_FT_FUNC( FTC_Manager_LookupFace )
	LOAD_FT_FUNC( FTC_Manager_LookupSize )

	LOAD_FT_FUNC( FT_Set_Char_Size )
	LOAD_FT_FUNC( FT_Load_Char )
	LOAD_FT_FUNC( FT_Done_Face )

	return true;
}

void UnloadFT()
{
	if ( hFTDll == NULL ) { return; }

#if defined(_WIN32)
	FreeLibrary( hFTDll );
#else
#  if !defined(FT_STATIC_LINK)
	// dynamic link almost everywhere
	dlclose( hFTDll );
#  endif
#endif
	hFTDll = NULL;
}
