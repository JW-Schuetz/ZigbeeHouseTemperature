
#pragma once

#include "Curl.h"


class CurlHandle
{
public:
    CurlHandle();
    virtual ~CurlHandle();

    void perform();

protected:
    CURL *getHandle();

private:
    CURL *handle;
};