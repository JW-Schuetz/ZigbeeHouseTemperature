
#pragma once

#include <string>
#include "CurlStaticLib.h"
#include "Curl.h"


using std::string;

// handle network attached storage (sending the data file)
class NAS: public Curl
{
public:
    NAS( string, string );
    ~NAS();

private:
    FILE *localFile;
};