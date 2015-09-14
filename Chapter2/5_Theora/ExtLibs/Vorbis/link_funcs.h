

#define OGG_DYNAMIC_LINK 1

#if OGG_DYNAMIC_LINK
typedef int  ( *ov_clear_func )( OggVorbis_File* vf );
typedef int  ( *ov_open_callbacks_func )( void* datasource, OggVorbis_File* vf, char* initial, long ibytes, ov_callbacks callbacks );
typedef int  ( *ov_time_seek_func )( OggVorbis_File* vf, double pos );
typedef long ( *ov_read_func )( OggVorbis_File* vf, char* buffer, int length, int bigendianp, int word, int sgned, int* bitstream );

typedef vorbis_info *( *ov_info_func )( OggVorbis_File* vf, int link );
typedef vorbis_comment *( *ov_comment_func )( OggVorbis_File* vf, int link );

ov_clear_func          OGG_ov_clear;
ov_open_callbacks_func OGG_ov_open_callbacks;
ov_time_seek_func      OGG_ov_time_seek;
ov_info_func           OGG_ov_info;
ov_comment_func        OGG_ov_comment;
ov_read_func           OGG_ov_read;
#endif

#if OGG_DYNAMIC_LINK
OGG_ov_read           = ( ov_read_func )FOGGLibrary->GetProcAddress( "ov_read" );
OGG_ov_info           = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_info" );
OGG_ov_comment        = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_comment" );
OGG_ov_time_seek      = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_time_seek" );
OGG_ov_open_callbacks = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_open_callbacks" );
OGG_ov_clear          = ( ov_info_func )FOGGLibrary->GetProcAddress( "ov_clear" );
#endif

#if OGG_DYNAMIC_LINK
FOGGLibrary->DisposeObject();
#endif

#if OGG_DYNAMIC_LINK
// dynamic link : use LoaderPointer ?
#else
// static link
#define OGG_ov_clear           ov_clear
#define OGG_ov_open_callbacks  ov_open_callbacks
#define OGG_ov_time_seek       ov_time_seek
#define OGG_ov_info            ov_info
#define OGG_ov_comment         ov_comment
#define OGG_ov_read            ov_read
#endif
