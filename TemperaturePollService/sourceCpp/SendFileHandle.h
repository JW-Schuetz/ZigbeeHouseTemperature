
#pragma once

#include <string>
#include "Curl.h"

using std::string;


class SendFileHandle
{
public:
    SendFileHandle( string, string );
    ~SendFileHandle();

    CURL *getHandle();

private:
    CURL *handle;
    FILE *fileToSend;
};