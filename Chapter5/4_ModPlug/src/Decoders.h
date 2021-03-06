/*
 * Copyright (C) 2013 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013 Viktor Latypov (vl@linderdaum.com)
 * Based on Linderdaum Engine http://www.linderdaum.com
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

#ifndef __OGG_inl__
#define __OGG_inl__

#include "modplug/modplug.h"

#if defined(ANDROID)
#  define ModPlug_Load_P          ModPlug_Load
#  define ModPlug_Unload_P        ModPlug_Unload
#  define ModPlug_Read_P          ModPlug_Read
#  define ModPlug_Seek_P          ModPlug_Seek
#  define ModPlug_GetSettings_P   ModPlug_GetSettings
#  define ModPlug_SetSettings_P   ModPlug_SetSettings
#else

typedef ModPlugFile* ( *ModPlug_Load_func )( const void* data, int size );
typedef void ( *ModPlug_Unload_func )( ModPlugFile* file );
typedef int  ( *ModPlug_Read_func )( ModPlugFile* file, void* buffer, int size );
typedef void ( *ModPlug_Seek_func )( ModPlugFile* file, int millisecond );
typedef void ( *ModPlug_GetSettings_func )( ModPlug_Settings* settings );
typedef void ( *ModPlug_SetSettings_func )( const ModPlug_Settings* settings );

extern ModPlug_Load_func ModPlug_Load_P;
extern ModPlug_Unload_func ModPlug_Unload_P;
extern ModPlug_Read_func ModPlug_Read_P;
extern ModPlug_Seek_func ModPlug_Seek_P;
extern ModPlug_GetSettings_func ModPlug_GetSettings_P;
extern ModPlug_SetSettings_func ModPlug_SetSettings_P;

#endif // OGG_DYNAMIC_LINK

bool LoadModPlug();
bool UnloadModPlug();

#endif
