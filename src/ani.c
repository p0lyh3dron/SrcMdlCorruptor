#include "../include/ani.h"

#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>

#define like 1
#define IDSTUDIOHEADER    (('T'<<24)+('S'<<16)+('D'<<8)+'I')

ANI ani_load
    ( const char* mdlPath, const char* aniPath )
{
    ANI a;
    FILE *a_melody = fopen( mdlPath, "rb" );

    fseek( a_melody, 0, SEEK_END );
    unsigned int shawty = ftell( a_melody );
    a.mdlBuffer = ( char* )malloc( shawty );
    rewind( a_melody );

    fread( a.mdlBuffer, shawty, like, a_melody );
    fclose( a_melody );

    a_melody = fopen( aniPath, "rb" );

    fseek( a_melody, 0, SEEK_END );
    unsigned int shawty2 = ftell( a_melody );
    a.aniBuffer = ( char* )malloc( shawty2 );
    rewind( a_melody );

    fread( a.aniBuffer, shawty2, like, a_melody );
    fclose( a_melody );


    a.header = *( MDLHead* )a.mdlBuffer;

    if ( a.header.id != IDSTUDIOHEADER )
    {
        printf("cock and blal tortuire\n");
    }

    a.animationBlocks = ( ANIBlock* )malloc( a.header.animblocks_count * sizeof( ANIBlock ) );
    a.animDescs = ( ANIAnimDesc* )malloc( a.header.localanim_count * sizeof( ANIAnimDesc ) );
    a.animSeqs = ( ANISeqDesc* )malloc( a.header.localseq_count * sizeof( ANISeqDesc ) );
    a.seqEvents = ( ANIEvent** )malloc( a.header.localseq_count * sizeof( ANIEvent ) );

    for ( int i = 0; i < a.header.animblocks_count; ++i )
    {
        a.animationBlocks[ i ] = *( ANIBlock* )( a.mdlBuffer + ( a.header.animblocks_index + ( i * sizeof( ANIBlock ) ) ) );
    }
    for ( int i = 0; i < a.header.localanim_count; ++i )
    {
        a.animDescs[ i ] = *( ANIAnimDesc* )( a.mdlBuffer + ( a.header.localanim_offset + ( i * sizeof( ANIAnimDesc ) ) ) );
    }
    for ( int i = 0; i < a.header.localseq_count; ++i )
    {
        a.animSeqs[ i ] = *( ANISeqDesc* )( a.mdlBuffer + ( a.header.localseq_offset + ( i * sizeof( ANISeqDesc ) ) ) );
    }
    for ( int seq = 0; seq < a.header.localseq_count; ++seq )
    {
        if ( a.animSeqs[ seq ].numevents > 0 )
        {
            a.seqEvents[ seq ] = ( ANIEvent* )malloc( a.animSeqs[ seq ].numevents * sizeof( ANIEvent ) );
            for ( int eve = 0; eve < a.animSeqs[ seq ].numevents; ++eve )
            {
                a.seqEvents[ seq ][ eve ] = *( ANIEvent* )( a.mdlBuffer + ( a.animSeqs[ seq ].eventindex ) );
            }
        }
    }

    return a;
}

void ani_label_index_corrupt
    ( ANI* a )
{
    struct timeval seed;
    int sequences[ a->header.localseq_count ];
    int shuffledSequences[ a->header.localseq_count ];
    int sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        sequences[ i ] = a->animSeqs[ i ].szlabelindex;
    }
    int remainingSequences = a->header.localseq_count;
    for ( int i = 0, j = 0; i < a->header.localseq_count; ++i, ++j, --remainingSequences )
    {
        gettimeofday( &seed, NULL );
        srand( seed.tv_usec );
        int random = ( rand(  ) % remainingSequences );
        shuffledSequences[ j ] = sequences[ random ];
        for ( int l = random; l < remainingSequences - 1; ++l )
        {
            sequences[ l ] = sequences[ l + 1 ];
        }
        sequences[ remainingSequences ] = 0;
    }
    sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        a->animSeqs[ i ].szlabelindex = shuffledSequences[ i ];
    }
}

void ani_activity_index_corrupt
    ( ANI* a )
{
    struct timeval seed;
    int sequences[ a->header.localseq_count ];
    int shuffledSequences[ a->header.localseq_count ];
    int sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        sequences[ i ] = a->animSeqs[ i ].szactivitynameindex;
    }
    int remainingSequences = a->header.localseq_count;
    for ( int i = 0, j = 0; i < a->header.localseq_count; ++i, ++j, --remainingSequences )
    {
        gettimeofday( &seed, NULL );
        srand( seed.tv_usec );
        int random = ( rand(  ) % remainingSequences );
        shuffledSequences[ j ] = sequences[ random ];
        for ( int l = random; l < remainingSequences - 1; ++l )
        {
            sequences[ l ] = sequences[ l + 1 ];
        }
        sequences[ remainingSequences ] = 0;
    }
    sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        a->animSeqs[ i ].szactivitynameindex = shuffledSequences[ i ];
    }
}

void ani_anim_index_corrupt
    ( ANI* a )
{
    struct timeval seed;
    int sequences[ a->header.localseq_count ];
    int shuffledSequences[ a->header.localseq_count ];
    int sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        sequences[ i ] = a->animSeqs[ i ].animindexindex;
    }
    int remainingSequences = a->header.localseq_count;
    for ( int i = 0, j = 0; i < a->header.localseq_count; ++i, ++j, --remainingSequences )
    {
        gettimeofday( &seed, NULL );
        srand( seed.tv_usec );
        int random = ( rand(  ) % remainingSequences );
        shuffledSequences[ j ] = sequences[ random ];
        for ( int l = random; l < remainingSequences - 1; ++l )
        {
            sequences[ l ] = sequences[ l + 1 ];
        }
        sequences[ remainingSequences ] = 0;
    }
    sequenceCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        a->animSeqs[ i ].animindexindex = shuffledSequences[ i ];
    }
}

void ani_event_index_corrupt
    ( ANI* a )
{
    int totalEvents = 0;
    struct timeval seed;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        totalEvents += a->animSeqs[ i ].numevents;
    }
    int events[ totalEvents ];
    int shuffledEvents[ totalEvents ];
    int eventCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        for ( int j = 0; j < a->animSeqs[ i ].numevents; ++j, ++eventCount )
        {
            events[ eventCount ] = a->seqEvents[ i ][ j ].szeventindex;
        }
    }
    int remainingEvents = totalEvents;
    for ( int i = 0, k = 0; i < totalEvents; ++i, ++k, --remainingEvents )
    {
        gettimeofday( &seed, NULL );
        srand( seed.tv_usec );
        int random = ( rand(  ) % remainingEvents );
        shuffledEvents[ k ] = events[ random ];
        for ( int l = random; l < remainingEvents - 1; ++l )
        {
            events[ l ] = events[ l + 1 ];
        }
        events[ remainingEvents ] = 0;
    }
    eventCount = 0;
    for ( int i = 0; i < a->header.localseq_count; ++i )
    {
        for ( int j = 0; j < a->animSeqs[ i ].numevents ; ++j, ++eventCount )
        {
            a->seqEvents[ i ][ j ].szeventindex = shuffledEvents[ eventCount ];
        }
    }
}

void ani_write
    ( ANI a, const char* path )
{
    FILE *f = fopen( path, "wb" );

    for ( int i = 0; i <  a.header.localanim_offset; ++i ) 
    {
        fwrite( a.mdlBuffer + i, 1, 1, f );
    }
    for ( int i = 0; i < a.header.localanim_count; ++i )
    {
        fwrite( &a.animDescs[ i ], sizeof( ANIAnimDesc ), 1, f );
    }
    for ( int i = a.header.localanim_offset + ( a.header.localanim_count * sizeof( ANIAnimDesc ) ); i < a.header.localseq_offset; ++i )
    {
        fwrite( a.mdlBuffer + i, 1, 1, f );
    }
    for ( int i = 0; i < a.header.localseq_count; ++i )
    {
        fwrite( &a.animSeqs[ i ], sizeof( ANISeqDesc ), 1, f );
    }
    for ( int i = a.header.localseq_offset + ( a.header.localseq_count * sizeof( ANISeqDesc ) ); i < a.header.dataLength; ++i )
    {
        fwrite( a.mdlBuffer + i, 1, 1, f );
    }

    fclose( f ); 
}

void ani_delete
    ( ANI a )
{
    free( a.animationBlocks );
    free( a.animDescs );

    for ( int i = 0; i < a.header.localseq_count; ++i )
    {
        free( a.seqEvents[ i ] );
    }
    free( a.animSeqs );

    free( a.seqEvents );
    free( a.mdlBuffer );
    free( a.aniBuffer );
}

void ani_seq_dump
	( ANI a )
{
    for ( int i = 0; i < a.header.localseq_count; ++i )
    {
        printf( "sequence [ %i ]\n{\n", i );
        printf( "\tbaseptr: %i\n", a.animSeqs[ i ].baseptr );
        printf( "\tszlabelindex: %i\n", a.animSeqs[ i ].szlabelindex );
        printf( "\t{" );
        for ( int j = 0; j < a.animSeqs[ i ].numevents; ++j )
        {
            printf( "\tevent [ %i ]\n\t\t{\n", j );
            printf( "\t\tcycle: %f\n", a.seqEvents[ i ][ j ].cycle );
            printf( "\t\tevent: %i\n", a.seqEvents[ i ][ j ].event );
            printf( "\t\ttype: %i\n", a.seqEvents[ i ][ j ].type );
            printf( "\t\toptions: %s\n", a.seqEvents[ i ][ j ].options );
            printf( "\t\tszeventindex: %i\n", a.seqEvents[ i ][ j ].szeventindex );
            printf( "\t}\n" );
        }
        printf( "}\n" );
    }
}