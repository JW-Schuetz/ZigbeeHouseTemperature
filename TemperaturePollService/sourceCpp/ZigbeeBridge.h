
#pragma once

#include <string>
#include <vector>
#include "CurlStaticLib.h"
#include "Curl.h"


using std::string;
using std::vector;


typedef struct WriteMemoryStruct
{
    vector<char> memory;
    int size;
} WriteMemoryStruct;


// handle zigbee bridge (reading raw data string of all available sensors)
class ZigbeeBridge: public Curl
{
public:
    ZigbeeBridge();

    string getRawDataString();                                  // get read raw data string

private:
    static size_t writeData( void *, size_t, size_t, void * );  // writefunction for curl

    static WriteMemoryStruct content;                           // storage for curl writefunction
};