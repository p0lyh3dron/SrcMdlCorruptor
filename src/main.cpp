#include "commandline.h"
#include "util.h"

#include "mdl.h"


void vec3_test()
{
	vec3 test;
	test.x = 4.f;
	test.y = 134.f;
	test.z = 234.f;

	vec3 test2;
	test2.x = 58.f;
	test2.y = 4568.f;
	test2.z = -23590.f;

	bool yate = test == test2;

	float bruh = test[1];
}


auto main( const int argc, char** argv ) -> int
{
	srand( time( 0 ) );

	cmdline.Init( argc, argv );

	// vec3_test();

	const std::string& mdlPath = cmdline.GetValue( "-i" );
	if ( mdlPath == "" )
	{
		fputs( "Error: no input path specified! (-i PATH)\n", stdout );
		return 1;
	}

	const std::string& mdlOut = cmdline.GetValue( "-o" );
	if ( mdlOut == "" )
	{
		fputs( "Error: no output path specified! (-o PATH)\n", stdout );
		return 1;
	}

	Model* test = mdl_load( mdlPath );

	if ( !test )
		return 1;

	mdl_randomize_fps( test );

	mdl_write( test, mdlOut.c_str() );

	printf( "Model written to \"%s\"\n", mdlOut.c_str() );

	return 0;
}

