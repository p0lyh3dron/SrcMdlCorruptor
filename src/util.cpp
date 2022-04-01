#include "util.h"

#include <stdarg.h>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <filesystem>

#include <sys/stat.h>


#ifdef _WIN32
	//#include <direct.h>
	//#include <sysinfoapi.h>
	#include <io.h>

	// get rid of the dumb windows posix depreciation warnings
	#define mkdir _mkdir
	#define chdir _chdir
	#define access _access
#elif __linux__
	#include <stdlib.h>
	#include <unistd.h>

	// windows-specific mkdir() is used
	#define mkdir(f) mkdir(f, 666)
#endif


void str_upper( std::string &string )
{
	std::transform( string.begin(), string.end(), string.begin(), ::toupper );
}

void str_lower( std::string &string )
{
	std::transform( string.begin(), string.end(), string.begin(), ::tolower);
}


std::string str_upper2( const std::string &in )
{
	std::string string = in;
	std::transform( string.begin(), string.end(), string.begin(), ::toupper );
	return string;
}

std::string str_lower2( const std::string &in )
{
	std::string string = in;
	std::transform( string.begin(), string.end(), string.begin(), ::tolower );
	return string;
}


// very cool
// https://stackoverflow.com/questions/55424746/is-there-an-analogous-function-to-vsnprintf-that-works-with-stdstring
void vstring( std::string& result, const char* format, ... )
{
	va_list args, args_copy;

	va_start( args, format );
	va_copy( args_copy, args );

	int len = vsnprintf( nullptr, 0, format, args );
	if ( len < 0 )
	{
		va_end( args_copy );
		va_end( args );
		fprintf( stderr, "vstring va_args: vsnprintf failed\n" );
		return;
	}

	if ( len > 0 )
	{
		result.resize( len );
		vsnprintf( result.data(), len+1, format, args_copy );
	}

	va_end( args_copy );
	va_end( args );
}

void vstring( std::string& s, const char* format, va_list args )
{
	va_list copy;
	va_copy( copy, args );
	int len = std::vsnprintf( nullptr, 0, format, copy );
	va_end( copy );

	if ( len >= 0 )
	{
		//std::string s( std::size_t(len) + 1, '\0' );
		s.resize( std::size_t(len) + 1, '\0' );
		std::vsnprintf( s.data(), s.size(), format, args );
		s.resize( len );
		return;
	}

	fprintf( stderr, "vstring va_list: vsnprintf failed\n" );
}


// very cool
// https://stackoverflow.com/questions/55424746/is-there-an-analogous-function-to-vsnprintf-that-works-with-stdstring
std::string vstring( const char* format, ... )
{
	std::string result;
	va_list args, args_copy;

	va_start( args, format );
	va_copy( args_copy, args );

	int len = vsnprintf( nullptr, 0, format, args );
	if (len < 0)
	{
		va_end(args_copy);
		va_end(args);
		throw std::runtime_error("vsnprintf error");
	}

	if ( len > 0 )
	{
		result.resize( len );
		vsnprintf( result.data(), len+1, format, args_copy );
	}

	va_end( args_copy );
	va_end( args );

	return result;
}

std::string vstring( const char* format, va_list args )
{
	va_list copy;
	va_copy( copy, args );
	int len = std::vsnprintf( nullptr, 0, format, copy );
	va_end( copy );

	if ( len >= 0 )
	{
		std::string s( std::size_t(len) + 1, '\0' );
		std::vsnprintf( s.data(), s.size(), format, args );
		s.resize( len );
		return s;
	}

	throw std::runtime_error( "vsnprintf error" );
}


double ToDouble( const std::string& value, double prev )
{
	if ( value.empty() )
		return prev;

	char* end;
	double result = strtod( value.c_str(), &end );

	return end == value.c_str() ? prev : result;
}


long ToLong( const std::string& value, int prev )
{
	if ( value.empty() )
		return prev;

	char* end;
	long result = strtol( value.c_str(), &end, 10 );

	return end == value.c_str() ? prev : result;
}


bool ToDouble2( const std::string &value, double &out )
{
	if ( value.empty() )
		return false;

	char *end;
	out = strtod( value.c_str(), &end );

	return end != value.c_str();
}


bool ToLong2( const std::string &value, long &out )
{
	if ( value.empty() )
		return false;

	char *end;
	out = strtol( value.c_str(), &end, 10 );

	return end != value.c_str();
}


// very cool: https://stackoverflow.com/a/46424921/12778316
std::string ToString( float value )
{
	std::ostringstream oss;
	oss << std::setprecision(8) << std::noshowpoint << value;
	return oss.str();
}


// ==============================================================================
// Filesystem functions


bool fs_is_file( const char* path )
{
	struct stat s;
	if ( stat( path, &s ) == 0 )
		return (s.st_mode & S_IFREG);

	return false;
}


bool fs_exists( const char* path )
{
	return (access( path, 0 ) != -1);
}


// BROKEN !!!!
std::string fs_get_filename( const std::string &path )
{
	// NOTE: could probably be faster with some strchr func, idk
	size_t i = path.length();
	for ( ; i > 0; i-- )
	{
		if ( path[i] == '/' || path[i] == '\\' )
			break;
	}

	// +1 is breaking it if i just pass in a filename? hmm, idk
	return path.substr( std::min(i+1, path.length()), path.length() );
}


std::string fs_get_file_ext( const std::string &path )
{
	const char *dot = strrchr( path.c_str(), '.' );
	if ( !dot || dot == path )
		return "";

	return dot + 1;
}


std::string fs_remove_ext( const std::string& path )
{
	size_t i = path.length();
	for ( ; i > 0; i-- )
	{
		if ( path[i] == '.' )
			break;
	}

	return path.substr( 0, i );
}

