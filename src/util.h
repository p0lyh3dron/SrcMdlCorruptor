#pragma once

/*
 * 
 * File for assorted utility functions
 * 
 */

#include <vector>
#include <string>
/* Linux memcmp.  */
#include <cstring>
#include <algorithm>

#include <time.h>

#if __linux__
#include <sys/stat.h>
#endif /* __linux__  */


// ==============================================================================
// Helper Macros

#define MALLOC_NEW( type ) (type*)malloc(sizeof(struct type))

#define ARR_SIZE( arr ) (sizeof(arr) / sizeof(arr[0]))

// much faster alternative to dynamic_cast
#define IS_TYPE( var1, var2 ) typeid(var1) == typeid(var2)


// ==============================================================================
// Shorter Types

using byte = unsigned char;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8  = char;
using s16 = short;
using s32 = int;
using s64 = long long;

using f32 = float;
using f64 = double;


// ==============================================================================
// Helper Functions for std::vector

#define NEW_VEC_INDEX 1

template <class T>
constexpr size_t vec_index( std::vector<T> &vec, T item, size_t fallback = SIZE_MAX )
{
#if NEW_VEC_INDEX
	auto it = std::find( vec.begin(), vec.end(), item );
	if ( it != vec.end() )
		return it - vec.begin();
#else
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] == item)
			return i;
	}
#endif

	return fallback;
}


template <class T>
constexpr size_t vec_index( const std::vector<T> &vec, T item, size_t fallback = SIZE_MAX )
{
#if NEW_VEC_INDEX
	auto it = std::find( vec.begin(), vec.end(), item );
	if ( it != vec.end() )
		return it - vec.begin();
#else
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] == item)
			return i;
	}
#endif

	return fallback;
}


template <class T>
constexpr void vec_remove( std::vector<T> &vec, T item )
{
	vec.erase( vec.begin() + vec_index( vec, item ) );
}


// Remove item if it exists
template <class T>
constexpr void vec_remove_if( std::vector<T> &vec, T item )
{
	size_t index = vec_index( vec, item );
	if ( index != SIZE_MAX )
		vec.erase( vec.begin() + index );
}


template <class T>
constexpr void vec_remove_index( std::vector<T> &vec, size_t index )
{
	vec.erase( vec.begin() + index );
}


template <class T>
constexpr bool vec_contains(std::vector<T> &vec, T item)
{
	return (std::find(vec.begin(), vec.end(), item) != vec.end());
}


template <class T>
constexpr bool vec_contains(const std::vector<T> &vec, T item)
{
	return (std::find(vec.begin(), vec.end(), item) != vec.end());
}


// ==============================================================================
// String Tools

void           str_upper( std::string &string );
void           str_lower( std::string &string );

// would like better names for this, but oh well
std::string    str_upper2( const std::string &in );
std::string    str_lower2( const std::string &in );

// don't need to worry about any resizing with these, but is a little slower
void           vstring( std::string& output, const char* format, ... );
void           vstring( std::string& output, const char* format, va_list args );

std::string    vstring( const char* format, ... );
std::string    vstring( const char* format, va_list args );


#define VSTRING( out, format ) \
	va_list args; \
	va_start( args, format ); \
	vstring( out, format, args ); \
	va_end( args )


// ==============================================================================
// Assorted Number/String Functions

double          ToDouble( const std::string& value, double prev );
long            ToLong( const std::string& value, int prev );

bool            ToDouble2( const std::string &value, double &out );
bool            ToLong2( const std::string &value, long &out );

std::string     ToString( float value );


// ==============================================================================
// Filesystem functions


bool fs_is_file( const char* path );
bool fs_exists( const char* path );

std::string fs_get_filename( const std::string& path );
std::string fs_get_file_ext( const std::string& path );
std::string fs_remove_ext( const std::string& path );


// ==============================================================================
// Other useful types

class vec3
{
public:
	vec3( float x, float y, float z ):
		x(x), y(y), z(z)
	{}

	vec3()
	{}

	float x{}, y{}, z{};

	constexpr float operator[]( int i )
	{
		// index into memory where vars are stored, and clamp to not read garbage
		return *(&x + std::clamp( i, 0, 2 ));
	}

	constexpr void operator=( const vec3& other )
	{
		// Guard self assignment
		if (this == &other)
			return;

		std::memcpy( &x, &other.x, sizeof( vec3 ) );
	}

	constexpr bool operator==( const vec3& other )
	{
		// Guard self assignment
		if (this == &other)
			return true;

		return !(std::memcmp( &x, &other.x, sizeof( vec3 ) ));
	}
};

