
#include <string>
#include "Curl.h"


using std::string;


Curl::Curl()
{
    handle = curl_easy_init();
    if( handle == NULL ) throw( string { "Curl::Curl" } );
}


Curl::~Curl()
{
    curl_easy_cleanup( handle );
}


CURL *Curl::getHandle()
{
    return handle;
}


void Curl::perform()
{
    CURLcode cres = curl_easy_perform( handle );
    if( cres != CURLE_OK ) throw( string { "Curl::perform" } );
}