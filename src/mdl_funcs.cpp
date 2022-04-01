#include "mdl_funcs.h"


#define IDSTUDIOHEADER    (('T'<<24)+('S'<<16)+('D'<<8)+'I')


int read_file( const char* path, std::vector< char >& buffer )
{
	FILE* handle = fopen( path, "rb");
	if ( !handle )
	{
		fprintf( stderr, "Failed to open file: \"%s\"\n", path );
		return 0;
	}

	fseek( handle, 0, SEEK_END );
	unsigned int size = ftell( handle );

	buffer.resize( size );
	rewind( handle );

	fread( buffer.data(), size, 1, handle);
	fclose( handle );

	// uh where do i put null terminator, here?
	// buffer[size] = '\0';

	return 0;
}


void mdl_alloc_animations( Model* mdl )
{
	// allocate information for the animation parts of it
	mdl->aAnimBlocks = ( ANIBlock* )malloc( mdl->aHeader->animblocks_count * sizeof( ANIBlock ) );
	mdl->aAnimDescs = ( ANIAnimDesc* )malloc( mdl->aHeader->localanim_count * sizeof( ANIAnimDesc ) );
	mdl->aAnimSeqs = ( ANISeqDesc* )malloc( mdl->aHeader->localseq_count * sizeof( ANISeqDesc ) );
	mdl->aSeqEvents = ( ANIEvent** )malloc( mdl->aHeader->localseq_count * sizeof( ANIEvent ) );

	// load animation blocks
	for ( int i = 0; i < mdl->aHeader->animblocks_count; ++i )
	{
		mdl->aAnimBlocks[ i ] = *( ANIBlock* )( mdl->aBufferMdl.data() + (mdl->aHeader->animblocks_index + (i * sizeof(ANIBlock))));
	}

	// load animation descriptors
	for ( int i = 0; i < mdl->aHeader->localanim_count; ++i )
	{
		mdl->aAnimDescs[ i ] = *( ANIAnimDesc* )( mdl->aBufferMdl.data() + ( mdl->aHeader->localanim_offset + ( i * sizeof( ANIAnimDesc ) ) ) );
	}

	// load animation sequences
	for ( int i = 0; i < mdl->aHeader->localseq_count; ++i )
	{
		mdl->aAnimSeqs[ i ] = *( ANISeqDesc* )( mdl->aBufferMdl.data() + ( mdl->aHeader->localseq_offset + ( i * sizeof( ANISeqDesc ) ) ) );
	}

	// load any events in animation sequences
	for ( int seq = 0; seq < mdl->aHeader->localseq_count; ++seq )
	{
		if ( mdl->aAnimSeqs[ seq ].numevents > 0 )
		{
			mdl->aSeqEvents[ seq ] = ( ANIEvent* )malloc( mdl->aAnimSeqs[ seq ].numevents * sizeof( ANIEvent ) );
			for ( int eve = 0; eve < mdl->aAnimSeqs[ seq ].numevents; ++eve )
			{
				mdl->aSeqEvents[ seq ][ eve ] = *( ANIEvent* )( mdl->aBufferMdl.data() + ( mdl->aAnimSeqs[ seq ].eventindex ) );
			}
		}
	}
}


Model* mdl_load( const std::string& path )
{
	Model* mdl = new Model;

	int ret = read_file( path.c_str(), mdl->aBufferMdl );

	if ( path.ends_with( ".mdl" ) )
	{
		std::string aniPath = fs_remove_ext( path );
		aniPath += ".ani";

		if ( fs_is_file( aniPath.c_str() ) )
		{
			mdl->aHasAni = read_file( path.c_str(), mdl->aBufferAni );
		}
	}

	// check header of file
	mdl->aHeader = ( MDLHeader* )mdl->aBufferMdl.data();

	if ( mdl->aHeader->id != IDSTUDIOHEADER )
	{
		printf( "Model does not have a valid header: \"%s\"\n", path.c_str() );
		delete mdl;
		return nullptr;
	}

	mdl_alloc_animations( mdl );

	return mdl;
}


void mdl_write( Model* mdl, const char* path )
{
	FILE *f = fopen( path, "wb" );

	for ( int i = 0; i <  mdl->aHeader->localanim_offset; ++i ) 
	{
		fwrite( mdl->aBufferMdl.data() + i, 1, 1, f );
	}
	for ( int i = 0; i < mdl->aHeader->localanim_count; ++i )
	{
		fwrite( &mdl->aAnimDescs[ i ], sizeof( ANIAnimDesc ), 1, f );
	}

	for ( int i = mdl->aHeader->localanim_offset + ( mdl->aHeader->localanim_count * sizeof( ANIAnimDesc ) ); i < mdl->aHeader->localseq_offset; ++i )
	{
		fwrite( mdl->aBufferMdl.data() + i, 1, 1, f );
	}
	for ( int i = 0; i < mdl->aHeader->localseq_count; ++i )
	{
		fwrite( &mdl->aAnimSeqs[ i ], sizeof( ANISeqDesc ), 1, f );
	}

	for ( int i = mdl->aHeader->localseq_offset + ( mdl->aHeader->localseq_count * sizeof( ANISeqDesc ) ); i < mdl->aHeader->dataLength; ++i )
	{
		fwrite( mdl->aBufferMdl.data() + i, 1, 1, f );
	}

	fclose( f ); 
}


// ---------------------------------------------------------------------
// basic corrupting stuff


void incremental_corrupt( void *spBuf, int sStart, int sEnd, int sIncrement, int sIncrementCount )
{
	int *pBuf = ( int* )spBuf;

	for ( int i = sStart; i < sEnd; i += sIncrement )
	{
		pBuf[ i ] += sIncrementCount;
	}
}

void mdl_randomize_fps( Model* mdl )
{
	/*
	 *    For demez: Make a buffer the fps values, and corrupt this buffer, instead
	 *    of corrupting the animation descriptors. This might require some memory
	 *    spaghetti and extra functions, but it will only modify the fps values.
	 */
	incremental_corrupt( mdl->aAnimDescs, 0, mdl->aHeader->localanim_count, 32, rand() % 100 );
}


