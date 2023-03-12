
#pragma once

#include <string>
#include <vector>
#include "Curl.h"
#include "CurlHandle.h"


using std::string;
using std::vector;


typedef struct WriteMemoryStruct
{
    vector<char> memory;
    int size;
} WriteMemoryStruct;


class CurlReadZigbeeBridge: public CurlHandle
{
public:
    CurlReadZigbeeBridge();

    string getRawDataString();

private:
    static size_t writeData(void *, size_t, size_t, void *); // writefunction for curl

    static WriteMemoryStruct content;   // storage for curl writefunction
};