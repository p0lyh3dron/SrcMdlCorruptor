#pragma once

#include "util.h"
#include "mdl.h"


Model*      mdl_load( const std::string& path );
void        mdl_write( Model* mdl, const char* path );

// ---------------------------------------------------------------------
// basic corrupting stuff

void        mdl_randomize_fps( Model* mdl, const std::string& path );

