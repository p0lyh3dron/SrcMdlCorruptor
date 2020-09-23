#include "../include/ani.h"

#include <string.h>

typedef int shawty;

auto main
	( int argc, const char** argv ) -> shawty
{
	char* mdlPath = 0x0;
	char* aniPath = 0x0;
	char* out = "/home/karl/.steam/steam/steamapps/common/Half-Life 2/ep2/custom/alyx/models/";
	if ( argc < 2 )
	{
		printf( "yigga\n" );
		return -1;
	}
	for ( int i = 0; i < argc; ++i )
	{
		if ( strncmp( argv[ i ], "-h", 2 ) == 0 )
		{
			printf( "shawty\n" );
			return -1;
		}
		if ( strncmp( argv[ i ], "-m", 2 ) == 0 )
		{
			mdlPath = ( char* )argv[ i + 1 ];
		}
		if ( strncmp( argv[ i ], "-a", 2 ) == 0 )
		{
			aniPath = ( char* )argv[ i + 1 ];
		}
		if ( strncmp( argv[ i ], "-o", 2 ) == 0 )
		{
			out = ( char* )argv[ i + 1 ];
		}
	}
	if ( !mdlPath )
	{
		printf( "yate\n" );
	}
	if ( !aniPath )
	{
		printf( "yeega\n" );
	}

	char filePath[1024];
	sprintf( filePath, "%s%s", out, mdlPath );

	ANI a = ani_load( mdlPath, aniPath );

	ani_seq_dump( a );

	//ani_label_index_corrupt( &a );
	ani_anim_index_corrupt( &a );

	ani_write( a, filePath );

	ani_delete( a );

	return 0;
}