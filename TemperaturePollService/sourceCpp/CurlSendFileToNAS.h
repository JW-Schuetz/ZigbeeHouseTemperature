
#pragma once

#include <string>
#include "Curl.h"
#include "CurlHandle.h"


using std::string;


class CurlSendFileToNAS: public CurlHandle
{
public:
    CurlSendFileToNAS( string, string );
    ~CurlSendFileToNAS();

private:
    FILE *localFile;
};