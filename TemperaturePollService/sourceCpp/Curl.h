
#pragma once

#include "CurlStaticLib.h"


// handle needs of curl library
class Curl
{
public:
    Curl();
    virtual ~Curl();

    void perform();     // perform curls action

protected:
    CURL *getHandle();  // return curls easy-handle

private:
    CURL *handle;       // curls easy-handle
};