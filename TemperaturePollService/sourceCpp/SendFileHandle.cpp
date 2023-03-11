
#include <string>
#include "Credentials.h"
#include "SendFileHandle.h"



SendFileHandle::SendFileHandle( string localFileName, string remoteFileName )
{
    handle = curl_easy_init();
    if( handle == NULL ) throw( string { "SendFileHandle::SendFileHandle curl_easy_init" } );

    fileToSend = fopen( localFileName.c_str(), "r" );
    if( fileToSend == NULL ) throw( string { "SendFileHandle::SendFileHandle fopen" } );

    // set curls parameters for filetransfer to NAS
    curl_easy_setopt( handle, CURLOPT_URL, remoteFileName.c_str() );
    curl_easy_setopt( handle, CURLOPT_READDATA, (void *)fileToSend );
    curl_easy_setopt( handle, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );
    curl_easy_setopt( handle, CURLOPT_USERPWD, FTPCREDENTIALS );
}


SendFileHandle::~SendFileHandle()
{
    fclose( fileToSend );

    curl_easy_cleanup( handle );
}


CURL *SendFileHandle::getHandle()
{
    return handle;
}