/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
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

#include "ExtLibs/ogg/ogg.h"
#include "ExtLibs/Theora/codec.h"
#include "ExtLibs/Theora/theora.h"
#include "ExtLibs/Theora/theoradec.h"
#include "ExtLibs/Vorbis/codec.h"

// we support both dynamic and static_ linking
#define THEORA_DYNAMIC_LINK 1

#if defined( OS_ANDROID ) || defined( OS_BLACKBERRY ) || defined( OS_MACOSX ) || defined( OS_LINUX )
#  undef THEORA_DYNAMIC_LINK
#endif // OS_ANDROID

#if defined( OS_MACOSX ) || ( defined( OS_LINUX ) && !defined( OS_ANDROID ) && !defined( OS_BLACKBERRY ) )
// old macos gcc does not have this
#  define __cdecl
#endif

#if !THEORA_DYNAMIC_LINK

// static link

#define TH_version_string th_version_string
#define TH_version_number th_version_number
#define TH_granule_frame th_granule_frame
#define TH_granule_time th_granule_time
#define TH_packet_isheader th_packet_isheader
#define TH_packet_iskeyframe th_packet_iskeyframe
#define TH_info_init th_info_init
#define TH_info_clear th_info_clear
#define TH_comment_init th_comment_init
#define TH_comment_add th_comment_add
#define TH_comment_add_tag th_comment_add_tag
#define TH_comment_query th_comment_query
#define TH_comment_clear th_comment_clear
#define TH_comment_query_count th_comment_query_count
#define TH_decode_alloc th_decode_alloc
#define TH_decode_headerin th_decode_headerin
#define TH_setup_free th_setup_free
#define TH_decode_ctl th_decode_ctl
#define TH_decode_packetin th_decode_packetin
#define TH_decode_ycbcr_out th_decode_ycbcr_out
#define TH_decode_free th_decode_free
#define VORBIS_info_init vorbis_info_init
#define VORBIS_info_clear vorbis_info_clear
#define VORBIS_info_blocksize vorbis_info_blocksize
#define VORBIS_comment_init vorbis_comment_init
#define VORBIS_comment_add vorbis_comment_add
#define VORBIS_comment_add_tag vorbis_comment_add_tag
#define VORBIS_comment_query vorbis_comment_query
#define VORBIS_comment_clear vorbis_comment_clear
#define VORBIS_comment_query_count vorbis_comment_query_count
#define VORBIS_block_init vorbis_block_init
#define VORBIS_block_clear vorbis_block_clear
#define VORBIS_dsp_clear vorbis_dsp_clear
#define VORBIS_granule_time vorbis_granule_time
#define VORBIS_version_string vorbis_version_string
#define VORBIS_analysis_init vorbis_analysis_init
#define VORBIS_commentheader_out vorbis_commentheader_out
#define VORBIS_analysis_headerout vorbis_analysis_headerout
#define VORBIS_analysis_buffer vorbis_analysis_buffer
#define VORBIS_analysis_wrote vorbis_analysis_wrote
#define VORBIS_analysis_blockout vorbis_analysis_blockout
#define VORBIS_analysis vorbis_analysis
#define VORBIS_bitrate_addblock vorbis_bitrate_addblock
#define VORBIS_bitrate_flushpacket vorbis_bitrate_flushpacket
#define VORBIS_synthesis_idheader vorbis_synthesis_idheader
#define VORBIS_synthesis_headerin vorbis_synthesis_headerin
#define VORBIS_synthesis_init vorbis_synthesis_init
#define VORBIS_synthesis_restart vorbis_synthesis_restart
#define VORBIS_synthesis vorbis_synthesis
#define VORBIS_synthesis_trackonly vorbis_synthesis_trackonly
#define VORBIS_synthesis_blockin vorbis_synthesis_blockin
#define VORBIS_synthesis_pcmout vorbis_synthesis_pcmout
#define VORBIS_synthesis_lapout vorbis_synthesis_lapout
#define VORBIS_synthesis_read vorbis_synthesis_read
#define VORBIS_packet_blocksize vorbis_packet_blocksize
#define VORBIS_synthesis_halfrate vorbis_synthesis_halfrate
#define VORBIS_synthesis_halfrate_p vorbis_synthesis_halfrate_p
#define OGG_sync_init ogg_sync_init
#define OGG_sync_clear ogg_sync_clear
#define OGG_sync_reset ogg_sync_reset
#define OGG_sync_destroy ogg_sync_destroy
#define OGG_sync_check ogg_sync_check
#define OGG_sync_buffer ogg_sync_buffer
#define OGG_sync_wrote ogg_sync_wrote
#define OGG_sync_pageseek ogg_sync_pageseek
#define OGG_sync_pageout ogg_sync_pageout
#define OGG_stream_pagein ogg_stream_pagein
#define OGG_stream_packetout ogg_stream_packetout
#define OGG_stream_packetpeek ogg_stream_packetpeek
#define OGG_stream_init ogg_stream_init
#define OGG_stream_clear ogg_stream_clear
#define OGG_stream_reset ogg_stream_reset
#define OGG_stream_reset_serialno ogg_stream_reset_serialno
#define OGG_stream_destroy ogg_stream_destroy
#define OGG_stream_check ogg_stream_check
#define OGG_stream_eos ogg_stream_eos
#define OGG_page_checksum_set ogg_page_checksum_set
#define OGG_page_version ogg_page_version
#define OGG_page_continued ogg_page_continued
#define OGG_page_bos ogg_page_bos
#define OGG_page_eos ogg_page_eos
#define OGG_page_granulepos ogg_page_granulepos
#define OGG_page_serialno ogg_page_serialno
#define OGG_page_pageno ogg_page_pageno
#define OGG_page_packets ogg_page_packets
#define OGG_packet_clear ogg_packet_clear
#define THEORA_version_string theora_version_string
#define THEORA_version_number theora_version_number
#define THEORA_decode_header theora_decode_header
#define THEORA_decode_init theora_decode_init
#define THEORA_decode_packetin theora_decode_packetin
#define THEORA_decode_YUVout theora_decode_yuvout
#define THEORA_packet_isheader theora_packet_isheader
#define THEORA_packet_iskeyframe theora_packet_iskeyframe
#define THEORA_granule_shift theora_granule_shift
#define THEORA_granule_frame theora_granule_frame
#define THEORA_granule_time theora_granule_time
#define THEORA_info_init theora_info_init
#define THEORA_info_clear theora_info_clear
#define THEORA_clear theora_clear
#define THEORA_comment_init theora_comment_init
#define THEORA_comment_add theora_comment_add
#define THEORA_comment_add_tag theora_comment_add_tag
#define THEORA_comment_query theora_comment_query
#define THEORA_comment_query_count theora_comment_query_count
#define THEORA_comment_clear theora_comment_clear
#define THEORA_control theora_control

#endif


#if defined(THEORA_DYNAMIC_LINK)

#include <windows.h>

typedef const char*  ( __cdecl* th_version_string_func )( void );
typedef ogg_uint32_t ( __cdecl* th_version_number_func )( void );
typedef ogg_int64_t  ( __cdecl* th_granule_frame_func )( void* _encdec, ogg_int64_t _granpos );
typedef double       ( __cdecl* th_granule_time_func )( void* _encdec, ogg_int64_t _granpos );
typedef int  ( __cdecl* th_packet_isheader_func )( ogg_packet* _op );
typedef int  ( __cdecl* th_packet_iskeyframe_func )( ogg_packet* _op );
typedef void ( __cdecl* th_info_init_func )( th_info* _info );
typedef void ( __cdecl* th_info_clear_func )( th_info* _info );
typedef void ( __cdecl* th_comment_init_func )( th_comment* _tc );
typedef void ( __cdecl* th_comment_add_func )( th_comment* _tc, char* _comment );
typedef void ( __cdecl* th_comment_add_tag_func )( th_comment* _tc, char* _tag, char* _val );
typedef char* ( __cdecl* th_comment_query_func )( th_comment* _tc, char* _tag, int _count );
typedef int  ( __cdecl* th_comment_query_count_func )( th_comment* _tc, char* _tag );
typedef void ( __cdecl* th_comment_clear_func )( th_comment* _tc );
typedef th_dec_ctx* ( __cdecl* th_decode_alloc_func )( const th_info* _info, const th_setup_info* _setup );

typedef int  ( __cdecl* th_decode_headerin_func )( th_info* _info, th_comment* _tc, th_setup_info** _setup, ogg_packet* _op );
typedef void ( __cdecl* th_setup_free_func )( th_setup_info* _setup );
typedef int  ( __cdecl* th_decode_ctl_func )( th_dec_ctx* _dec, int _req, void* _buf, size_t _buf_sz );
typedef int  ( __cdecl* th_decode_packetin_func )( th_dec_ctx* _dec, const ogg_packet* _op, ogg_int64_t* _granpos );
typedef int  ( __cdecl* th_decode_ycbcr_out_func )( th_dec_ctx* _dec, th_ycbcr_buffer _ycbcr );
typedef void ( __cdecl* th_decode_free_func )( th_dec_ctx* _dec );

typedef int      ( __cdecl* ogg_sync_init_func )( ogg_sync_state* oy );
typedef int      ( __cdecl* ogg_sync_clear_func )( ogg_sync_state* oy );
typedef int      ( __cdecl* ogg_sync_reset_func )( ogg_sync_state* oy );
typedef int      ( __cdecl* ogg_sync_destroy_func )( ogg_sync_state* oy );
typedef int      ( __cdecl* ogg_sync_check_func )( ogg_sync_state* oy );
typedef char*    ( __cdecl* ogg_sync_buffer_func )( ogg_sync_state* oy, long size );
typedef int      ( __cdecl* ogg_sync_wrote_func )( ogg_sync_state* oy, long bytes );
typedef long     ( __cdecl* ogg_sync_pageseek_func )( ogg_sync_state* oy, ogg_page* og );
typedef int      ( __cdecl* ogg_sync_pageout_func )( ogg_sync_state* oy, ogg_page* og );
typedef int      ( __cdecl* ogg_stream_pagein_func )( ogg_stream_state* os, ogg_page* og );
typedef int      ( __cdecl* ogg_stream_packetout_func )( ogg_stream_state* os, ogg_packet* op );
typedef int      ( __cdecl* ogg_stream_packetpeek_func )( ogg_stream_state* os, ogg_packet* op );
typedef int      ( __cdecl* ogg_stream_init_func )( ogg_stream_state* os, int serialno );
typedef int      ( __cdecl* ogg_stream_clear_func )( ogg_stream_state* os );
typedef int      ( __cdecl* ogg_stream_reset_func )( ogg_stream_state* os );
typedef int      ( __cdecl* ogg_stream_reset_serialno_func )( ogg_stream_state* os, int serialno );
typedef int      ( __cdecl* ogg_stream_destroy_func )( ogg_stream_state* os );
typedef int      ( __cdecl* ogg_stream_check_func )( ogg_stream_state* os );
typedef int      ( __cdecl* ogg_stream_eos_func )( ogg_stream_state* os );
typedef void     ( __cdecl* ogg_page_checksum_set_func )( ogg_page* og );
typedef int      ( __cdecl* ogg_page_version_func )( const ogg_page* og );
typedef int      ( __cdecl* ogg_page_continued_func )( const ogg_page* og );
typedef int      ( __cdecl* ogg_page_bos_func )( const ogg_page* og );
typedef int      ( __cdecl* ogg_page_eos_func )( const ogg_page* og );
typedef ogg_int64_t  ( __cdecl* ogg_page_granulepos_func )( const ogg_page* og );
typedef int      ( __cdecl* ogg_page_serialno_func )( const ogg_page* og );
typedef long     ( __cdecl* ogg_page_pageno_func )( const ogg_page* og );
typedef int      ( __cdecl* ogg_page_packets_func )( const ogg_page* og );
typedef void     ( __cdecl* ogg_packet_clear_func )( ogg_packet* op );

typedef void     ( __cdecl* vorbis_info_init_func )( vorbis_info* vi );
typedef void     ( __cdecl* vorbis_info_clear_func )( vorbis_info* vi );
typedef int      ( __cdecl* vorbis_info_blocksize_func )( vorbis_info* vi, int zo );
typedef void     ( __cdecl* vorbis_comment_init_func )( vorbis_comment* vc );
typedef void     ( __cdecl* vorbis_comment_add_func )( vorbis_comment* vc, const char* comment );
typedef void     ( __cdecl* vorbis_comment_add_tag_func )( vorbis_comment* vc, const char* tag, const char* contents );
typedef char*    ( __cdecl* vorbis_comment_query_func )( vorbis_comment* vc, const char* tag, int count );
typedef int      ( __cdecl* vorbis_comment_query_count_func )( vorbis_comment* vc, const char* tag );
typedef void     ( __cdecl* vorbis_comment_clear_func )( vorbis_comment* vc );
typedef int      ( __cdecl* vorbis_block_init_func )( vorbis_dsp_state* v, vorbis_block* vb );
typedef int      ( __cdecl* vorbis_block_clear_func )( vorbis_block* vb );
typedef void     ( __cdecl* vorbis_dsp_clear_func )( vorbis_dsp_state* v );
typedef double   ( __cdecl* vorbis_granule_time_func )( vorbis_dsp_state* v, ogg_int64_t granulepos );
typedef const char* ( __cdecl* vorbis_version_string_func )( void );
typedef int      ( __cdecl* vorbis_analysis_init_func )( vorbis_dsp_state* v, vorbis_info* vi );
typedef int      ( __cdecl* vorbis_commentheader_out_func )( vorbis_comment* vc, ogg_packet* op );
typedef int      ( __cdecl* vorbis_analysis_headerout_func )( vorbis_dsp_state* v, vorbis_comment* vc, ogg_packet* op, ogg_packet* op_comm, ogg_packet* op_code );
typedef float**  ( __cdecl* vorbis_analysis_buffer_func )( vorbis_dsp_state* v, int vals );
typedef int      ( __cdecl* vorbis_analysis_wrote_func )( vorbis_dsp_state* v, int vals );
typedef int      ( __cdecl* vorbis_analysis_blockout_func )( vorbis_dsp_state* v, vorbis_block* vb );
typedef int      ( __cdecl* vorbis_analysis_func )( vorbis_block* vb, ogg_packet* op );
typedef int      ( __cdecl* vorbis_bitrate_addblock_func )( vorbis_block* vb );
typedef int      ( __cdecl* vorbis_bitrate_flushpacket_func )( vorbis_dsp_state* vd, ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_idheader_func )( ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_headerin_func )( vorbis_info* vi, vorbis_comment* vc, ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_init_func )( vorbis_dsp_state* v, vorbis_info* vi );
typedef int      ( __cdecl* vorbis_synthesis_restart_func )( vorbis_dsp_state* v );
typedef int      ( __cdecl* vorbis_synthesis_func )( vorbis_block* vb, ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_trackonly_func )( vorbis_block* vb, ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_blockin_func )( vorbis_dsp_state* v, vorbis_block* vb );
typedef int      ( __cdecl* vorbis_synthesis_pcmout_func )( vorbis_dsp_state* v, float** *pcm );
typedef int      ( __cdecl* vorbis_synthesis_lapout_func )( vorbis_dsp_state* v, float** *pcm );
typedef int      ( __cdecl* vorbis_synthesis_read_func )( vorbis_dsp_state* v, int samples );
typedef long     ( __cdecl* vorbis_packet_blocksize_func )( vorbis_info* vi, ogg_packet* op );
typedef int      ( __cdecl* vorbis_synthesis_halfrate_func )( vorbis_info* v, int flag );
typedef int      ( __cdecl* vorbis_synthesis_halfrate_p_func )( vorbis_info* v );

typedef const char* ( __cdecl* theora_version_string_func )( void );
typedef ogg_uint32_t ( __cdecl* theora_version_number_func )( void );
typedef int ( __cdecl* theora_decode_header_func )( theora_info* ci, theora_comment* cc, ogg_packet* op );
typedef int ( __cdecl* theora_decode_init_func )( theora_state* th, theora_info* c );
typedef int ( __cdecl* theora_decode_packetin_func )( theora_state* th, ogg_packet* op );
typedef int ( __cdecl* theora_decode_YUVout_func )( theora_state* th, yuv_buffer* yuv );
typedef int ( __cdecl* theora_packet_isheader_func )( ogg_packet* op );
typedef int ( __cdecl* theora_packet_iskeyframe_func )( ogg_packet* op );
typedef int ( __cdecl* theora_granule_shift_func )( theora_info* ti );
typedef ogg_int64_t ( __cdecl* theora_granule_frame_func )( theora_state* th, ogg_int64_t granulepos );
typedef double ( __cdecl* theora_granule_time_func )( theora_state* th, ogg_int64_t granulepos );
typedef void  ( __cdecl* theora_info_init_func )( theora_info* c );
typedef void  ( __cdecl* theora_info_clear_func )( theora_info* c );
typedef void  ( __cdecl* theora_clear_func )( theora_state* t );
typedef void  ( __cdecl* theora_comment_init_func )( theora_comment* tc );
typedef void  ( __cdecl* theora_comment_add_func )( theora_comment* tc, char* comment );
typedef void  ( __cdecl* theora_comment_add_tag_func )( theora_comment* tc, char* tag, char* value );
typedef char* ( __cdecl* theora_comment_query_func )( theora_comment* tc, char* tag, int count );
typedef int   ( __cdecl* theora_comment_query_count_func )( theora_comment* tc, char* tag );
typedef void  ( __cdecl* theora_comment_clear_func )( theora_comment* tc );
typedef int   ( __cdecl* theora_control_func )( theora_state* th, int req, void* buf, size_t buf_sz );

th_version_string_func  TH_version_string;
th_version_number_func  TH_version_number;
th_granule_frame_func   TH_granule_frame;
th_granule_time_func    TH_granule_time;
th_packet_isheader_func TH_packet_isheader;
th_packet_iskeyframe_func TH_packet_iskeyframe;
th_info_init_func  TH_info_init;
th_info_clear_func TH_info_clear;
th_comment_init_func    TH_comment_init;
th_comment_add_func     TH_comment_add;
th_comment_add_tag_func TH_comment_add_tag;
th_comment_query_func   TH_comment_query;
th_comment_clear_func   TH_comment_clear;
th_comment_query_count_func TH_comment_query_count;

th_decode_alloc_func     TH_decode_alloc;
th_decode_headerin_func  TH_decode_headerin;
th_setup_free_func       TH_setup_free;
th_decode_ctl_func       TH_decode_ctl;
th_decode_packetin_func  TH_decode_packetin;
th_decode_ycbcr_out_func TH_decode_ycbcr_out;
th_decode_free_func      TH_decode_free;

vorbis_info_init_func       VORBIS_info_init;
vorbis_info_clear_func      VORBIS_info_clear;
vorbis_info_blocksize_func  VORBIS_info_blocksize;
vorbis_comment_init_func    VORBIS_comment_init;
vorbis_comment_add_func     VORBIS_comment_add;
vorbis_comment_add_tag_func VORBIS_comment_add_tag;
vorbis_comment_query_func   VORBIS_comment_query;
vorbis_comment_clear_func   VORBIS_comment_clear;
vorbis_comment_query_count_func VORBIS_comment_query_count;
vorbis_block_init_func      VORBIS_block_init;
vorbis_block_clear_func     VORBIS_block_clear;
vorbis_dsp_clear_func       VORBIS_dsp_clear;
vorbis_granule_time_func    VORBIS_granule_time;
vorbis_version_string_func  VORBIS_version_string;
vorbis_analysis_init_func        VORBIS_analysis_init;
vorbis_commentheader_out_func    VORBIS_commentheader_out;
vorbis_analysis_headerout_func   VORBIS_analysis_headerout;
vorbis_analysis_buffer_func      VORBIS_analysis_buffer;
vorbis_analysis_wrote_func       VORBIS_analysis_wrote;
vorbis_analysis_blockout_func    VORBIS_analysis_blockout;
vorbis_analysis_func             VORBIS_analysis;
vorbis_bitrate_addblock_func     VORBIS_bitrate_addblock;
vorbis_bitrate_flushpacket_func  VORBIS_bitrate_flushpacket;
vorbis_synthesis_idheader_func   VORBIS_synthesis_idheader;
vorbis_synthesis_headerin_func   VORBIS_synthesis_headerin;
vorbis_synthesis_init_func       VORBIS_synthesis_init;
vorbis_synthesis_restart_func    VORBIS_synthesis_restart;
vorbis_synthesis_func            VORBIS_synthesis;
vorbis_synthesis_trackonly_func  VORBIS_synthesis_trackonly;
vorbis_synthesis_blockin_func    VORBIS_synthesis_blockin;
vorbis_synthesis_pcmout_func     VORBIS_synthesis_pcmout;
vorbis_synthesis_lapout_func     VORBIS_synthesis_lapout;
vorbis_synthesis_read_func       VORBIS_synthesis_read;
vorbis_packet_blocksize_func     VORBIS_packet_blocksize;
vorbis_synthesis_halfrate_func   VORBIS_synthesis_halfrate;
vorbis_synthesis_halfrate_p_func VORBIS_synthesis_halfrate_p;

ogg_sync_init_func       OGG_sync_init;
ogg_sync_clear_func      OGG_sync_clear;
ogg_sync_reset_func      OGG_sync_reset;
ogg_sync_destroy_func    OGG_sync_destroy;
ogg_sync_check_func      OGG_sync_check;
ogg_sync_buffer_func     OGG_sync_buffer;
ogg_sync_wrote_func      OGG_sync_wrote;
ogg_sync_pageseek_func   OGG_sync_pageseek;
ogg_sync_pageout_func    OGG_sync_pageout;
ogg_stream_pagein_func   OGG_stream_pagein;
ogg_stream_packetout_func  OGG_stream_packetout;
ogg_stream_packetpeek_func OGG_stream_packetpeek;
ogg_stream_init_func     OGG_stream_init;
ogg_stream_clear_func    OGG_stream_clear;
ogg_stream_reset_func    OGG_stream_reset;
ogg_stream_reset_serialno_func OGG_stream_reset_serialno;
ogg_stream_destroy_func  OGG_stream_destroy;
ogg_stream_check_func    OGG_stream_check;
ogg_stream_eos_func      OGG_stream_eos;
ogg_page_checksum_set_func OGG_page_checksum_set;
ogg_page_version_func    OGG_page_version;
ogg_page_continued_func  OGG_page_continued;
ogg_page_bos_func OGG_page_bos;
ogg_page_eos_func OGG_page_eos;
ogg_page_granulepos_func OGG_page_granulepos;
ogg_page_serialno_func   OGG_page_serialno;
ogg_page_pageno_func     OGG_page_pageno;
ogg_page_packets_func    OGG_page_packets;
ogg_packet_clear_func    OGG_packet_clear;

theora_version_string_func THEORA_version_string;
theora_version_number_func THEORA_version_number;
theora_decode_header_func  THEORA_decode_header;
theora_decode_init_func THEORA_decode_init;
theora_decode_packetin_func THEORA_decode_packetin;
theora_decode_YUVout_func THEORA_decode_YUVout;
theora_packet_isheader_func THEORA_packet_isheader;
theora_packet_iskeyframe_func THEORA_packet_iskeyframe;
theora_granule_shift_func THEORA_granule_shift;
theora_granule_frame_func THEORA_granule_frame;
theora_granule_time_func THEORA_granule_time;
theora_info_init_func THEORA_info_init;
theora_info_clear_func THEORA_info_clear;
theora_clear_func THEORA_clear;
theora_comment_init_func THEORA_comment_init;
theora_comment_add_func THEORA_comment_add;
theora_comment_add_tag_func THEORA_comment_add_tag;
theora_comment_query_func THEORA_comment_query;
theora_comment_query_count_func THEORA_comment_query_count;
theora_comment_clear_func THEORA_comment_clear;
theora_control_func THEORA_control;

HMODULE FThLibrary     = NULL;
HMODULE FTheoraLibrary = NULL;
HMODULE FOggLibrary    = NULL;
HMODULE FVorbisLibrary = NULL;

std::string LinkToTheora()
{
	// exit if the OGG library is already loaded
	if ( FThLibrary ) { return ""; }

	// _d.dll for DEBUG
	FVorbisLibrary = LoadLibraryA( "libvorbis.dll" );
	FOggLibrary = LoadLibraryA( "libogg.dll" );
	FThLibrary = LoadLibraryA( "libtheora.dll" );
	FTheoraLibrary = FThLibrary;

	if ( !FVorbisLibrary || !FOggLibrary || !FThLibrary )
	{
		if ( FVorbisLibrary ) { FreeLibrary( FVorbisLibrary ); }

		if ( FTheoraLibrary ) { FreeLibrary( FTheoraLibrary ); }

		if ( FOggLibrary )    { FreeLibrary( FOggLibrary ); }

		FTheoraLibrary = FThLibrary = NULL;
		FOggLibrary = NULL;
		FVorbisLibrary = NULL;

		return "Cannot load Ogg/Theora/Vorbis libraries";
	}

#define LOAD_TH_FUNC(func_name) \
   TH_##func_name = (th_##func_name##_func)GetProcAddress(FThLibrary, "th_"#func_name); \
   if(!TH_##func_name) \
   { \
      return std::string("Cannot load ") + std::string("th_"#func_name);\
   }

	LOAD_TH_FUNC( version_string );
	LOAD_TH_FUNC( version_number );
	LOAD_TH_FUNC( granule_frame );
	LOAD_TH_FUNC( granule_time );
	LOAD_TH_FUNC( packet_isheader );
	LOAD_TH_FUNC( packet_iskeyframe );
	LOAD_TH_FUNC( info_init );
	LOAD_TH_FUNC( info_clear );
	LOAD_TH_FUNC( comment_init );
	LOAD_TH_FUNC( comment_add );
	LOAD_TH_FUNC( comment_add_tag );
	LOAD_TH_FUNC( comment_query );
	LOAD_TH_FUNC( comment_query_count );
	LOAD_TH_FUNC( comment_clear );

	LOAD_TH_FUNC( decode_alloc );
	LOAD_TH_FUNC( decode_headerin );
	LOAD_TH_FUNC( setup_free );
	LOAD_TH_FUNC( decode_ctl );
	LOAD_TH_FUNC( decode_packetin );
	LOAD_TH_FUNC( decode_ycbcr_out );
	LOAD_TH_FUNC( decode_free );

#undef LOAD_TH_FUNC

#define LOAD_VORBIS_FUNC(func_name) \
   VORBIS_##func_name = (vorbis_##func_name##_func)GetProcAddress(FVorbisLibrary, "vorbis_"#func_name); \
   if(!VORBIS_##func_name) \
   { \
      return std::string("Cannot load ") + std::string("vorbis_"#func_name);\
   }

	LOAD_VORBIS_FUNC( info_init );
	LOAD_VORBIS_FUNC( info_clear );
	LOAD_VORBIS_FUNC( info_blocksize );
	LOAD_VORBIS_FUNC( comment_init );
	LOAD_VORBIS_FUNC( comment_add );
	LOAD_VORBIS_FUNC( comment_add_tag );
	LOAD_VORBIS_FUNC( comment_query );
	LOAD_VORBIS_FUNC( comment_query_count );
	LOAD_VORBIS_FUNC( comment_clear );
	LOAD_VORBIS_FUNC( block_init );
	LOAD_VORBIS_FUNC( block_clear );
	LOAD_VORBIS_FUNC( dsp_clear );
	LOAD_VORBIS_FUNC( granule_time );
	LOAD_VORBIS_FUNC( version_string );
	LOAD_VORBIS_FUNC( analysis_init );
	LOAD_VORBIS_FUNC( commentheader_out );
	LOAD_VORBIS_FUNC( analysis_headerout );
	LOAD_VORBIS_FUNC( analysis_buffer );
	LOAD_VORBIS_FUNC( analysis_wrote );
	LOAD_VORBIS_FUNC( analysis_blockout );
	LOAD_VORBIS_FUNC( analysis );
	LOAD_VORBIS_FUNC( bitrate_addblock );
	LOAD_VORBIS_FUNC( bitrate_flushpacket );
	LOAD_VORBIS_FUNC( synthesis_idheader );
	LOAD_VORBIS_FUNC( synthesis_headerin );
	LOAD_VORBIS_FUNC( synthesis_init );
	LOAD_VORBIS_FUNC( synthesis_restart );
	LOAD_VORBIS_FUNC( synthesis );
	LOAD_VORBIS_FUNC( synthesis_trackonly );
	LOAD_VORBIS_FUNC( synthesis_blockin );
	LOAD_VORBIS_FUNC( synthesis_pcmout );
	LOAD_VORBIS_FUNC( synthesis_lapout );
	LOAD_VORBIS_FUNC( synthesis_read );
	LOAD_VORBIS_FUNC( packet_blocksize );
	LOAD_VORBIS_FUNC( synthesis_halfrate );
	LOAD_VORBIS_FUNC( synthesis_halfrate_p );

#undef LOAD_VORBIS_FUNC

#define LOAD_OGG_FUNC(func_name) \
   OGG_##func_name = (ogg_##func_name##_func)GetProcAddress(FOggLibrary, "ogg_"#func_name); \
   if(!OGG_##func_name) \
   { \
      return std::string("Cannot load ") + std::string("ogg_"#func_name);\
   }

	LOAD_OGG_FUNC( sync_init );
	LOAD_OGG_FUNC( sync_clear );
	LOAD_OGG_FUNC( sync_reset );
	LOAD_OGG_FUNC( sync_destroy );
	//LOAD_OGG_FUNC(sync_check);
	LOAD_OGG_FUNC( sync_buffer );
	LOAD_OGG_FUNC( sync_wrote );
	LOAD_OGG_FUNC( sync_pageseek );
	LOAD_OGG_FUNC( sync_pageout );
	LOAD_OGG_FUNC( stream_pagein );
	LOAD_OGG_FUNC( stream_init );
	LOAD_OGG_FUNC( stream_clear );
	LOAD_OGG_FUNC( stream_reset );
	LOAD_OGG_FUNC( stream_destroy );
	//LOAD_OGG_FUNC(stream_check);
	LOAD_OGG_FUNC( stream_eos );
	LOAD_OGG_FUNC( stream_reset_serialno );
	LOAD_OGG_FUNC( stream_packetout );
	LOAD_OGG_FUNC( stream_packetpeek );
	LOAD_OGG_FUNC( page_checksum_set );
	LOAD_OGG_FUNC( page_version );
	LOAD_OGG_FUNC( page_continued );
	LOAD_OGG_FUNC( page_bos );
	LOAD_OGG_FUNC( page_eos );
	LOAD_OGG_FUNC( page_granulepos );
	LOAD_OGG_FUNC( page_serialno );
	LOAD_OGG_FUNC( page_pageno );
	LOAD_OGG_FUNC( page_packets );
	LOAD_OGG_FUNC( packet_clear );

#undef LOAD_OGG_FUNC

#define LOAD_THEORA_FUNC(func_name) \
   THEORA_##func_name = (theora_##func_name##_func)GetProcAddress(FTheoraLibrary, "theora_"#func_name); \
   if(!THEORA_##func_name) \
   { \
      return std::string("Cannot load ") + std::string("theora_"#func_name);\
   }

	LOAD_THEORA_FUNC( version_string );
	LOAD_THEORA_FUNC( version_number );
	LOAD_THEORA_FUNC( decode_header );
	LOAD_THEORA_FUNC( decode_init );
	LOAD_THEORA_FUNC( decode_packetin );
	LOAD_THEORA_FUNC( decode_YUVout );
	LOAD_THEORA_FUNC( packet_isheader );
	LOAD_THEORA_FUNC( packet_iskeyframe );
	LOAD_THEORA_FUNC( granule_shift );
	LOAD_THEORA_FUNC( granule_frame );
	LOAD_THEORA_FUNC( granule_time );
	LOAD_THEORA_FUNC( info_init );
	LOAD_THEORA_FUNC( info_clear );
	LOAD_THEORA_FUNC( clear );
	LOAD_THEORA_FUNC( comment_init );
	LOAD_THEORA_FUNC( comment_add );
	LOAD_THEORA_FUNC( comment_add_tag );
	LOAD_THEORA_FUNC( comment_query );
	LOAD_THEORA_FUNC( comment_query_count );
	LOAD_THEORA_FUNC( comment_clear );
	LOAD_THEORA_FUNC( control );

#undef LOAD_THEORA_FUNC

	return "";
}

#endif // THEORA_DYNAMIC_LINK
