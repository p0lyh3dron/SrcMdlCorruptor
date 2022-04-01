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
	cmdline.Init( argc, argv );

	vec3_test();

	return 0;
}

