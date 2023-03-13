
#include "Credentials.h"
#include "NAS.h"


NAS::NAS( string localFileName, string remoteFileName )
{
    // open local file for reading
    localFile = fopen( localFileName.c_str(), "r" );
    if( localFile == NULL ) throw( string { "NAS::NAS" } );

    CURL *handle = getHandle();

    // set curls parameters for filetransfer to NAS
    curl_easy_setopt( handle, CURLOPT_URL, remoteFileName.c_str() );
    curl_easy_setopt( handle, CURLOPT_READDATA, (void *)localFile );
    curl_easy_setopt( handle, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );
    curl_easy_setopt( handle, CURLOPT_USERPWD, FTPCREDENTIALS );
}


NAS::~NAS()
{
    fclose( localFile );
}