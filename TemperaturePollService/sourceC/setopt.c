
#include "sensors.h"


FILE *setftp( CURLM *easy_handle, char *filename )
{
    int n = strlen( DISKSTATION ) + strlen( filename ) + 1;
    char *remotefilename = (char *)malloc( n );
    memset( remotefilename, 0, n );

    remotefilename = strcat( remotefilename, DISKSTATION );
    remotefilename = strcat( remotefilename, filename );
 
    curl_easy_setopt( easy_handle, CURLOPT_URL, remotefilename );

    free( remotefilename );

    FILE *file = fopen( filename, "r" );
    curl_easy_setopt( easy_handle, CURLOPT_READDATA, (void *)file );

    curl_easy_setopt( easy_handle, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );

    curl_easy_setopt( easy_handle, CURLOPT_USERPWD, FTPCREDENTIALS );

    return( file );
}

void setpoll( CURLM *easy_handle, struct curl_slist **headers, struct MemoryStruct *content )
{
    curl_easy_setopt( easy_handle, CURLOPT_URL, URL );
    curl_easy_setopt( easy_handle, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEFUNCTION, write_data );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEDATA, (void *)content );

    // pass our list of custom made headers
    curl_easy_setopt( easy_handle, CURLOPT_HTTPHEADER, *headers );
}