
#include <string>
#include "CurlHandle.h"


using std::string;


CurlHandle::CurlHandle()
{
    handle = curl_easy_init();
    if( handle == NULL ) throw( string { "CurlHandle::CurlHandle" } );
}


CurlHandle::~CurlHandle()
{
    curl_easy_cleanup( handle );
}


CURL *CurlHandle::getHandle()
{
    return handle;
}


void CurlHandle::perform()
{
    CURLcode cres = curl_easy_perform( handle );
    if( cres != CURLE_OK ) throw( string { "CurlHandle::perform" } );
}