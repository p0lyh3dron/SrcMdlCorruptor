/*
commandline.h ( Authored by Demez )

Simple Parser for command line options
*/

#pragma once

#include <string>
#include <vector>


class CommandLine
{
private:
	std::vector< std::string >          aArgs;

public:

	void                                        Init( int argc, char *argv[] );
	int                                         GetIndex( const std::string& search );
	constexpr const std::vector<std::string>&   GetArgs(  );
	constexpr size_t                            GetCount(  );

	bool                                        Find( const std::string& search );

	const std::string&                          GetValue( const std::string& search, const std::string& fallback = "" );
	int                                         GetValue( const std::string& search, int fallback );
	float                                       GetValue( const std::string& search, float fallback );
	double                                      GetValue( const std::string& search, double fallback );

	// function to be able to find all values like this
	// returns true if it finds a value, false if it fails to
	bool                                        GetValueNext( int& index, const std::string& search, std::string& ret );
	
	CommandLine(  );
	~CommandLine(  );
};

extern CommandLine cmdline;

