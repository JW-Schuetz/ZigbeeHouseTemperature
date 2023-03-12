
#pragma once

#include "Curl.h"


class CurlHandle
{
public:
    CurlHandle();
    virtual ~CurlHandle();

    CURL *getHandle();
    void perform();

protected:
    CURL *handle;
};