
#include <signal.h>
#include "sensors.h"


struct MemoryStruct content;

void signalhandler( int sig, siginfo_t *si, void *uc )
{
    // 1: falls ein neuer Eintrag im Datenfile, 0: sonst
    CURLM *easy_handle;
    int file_written = 0;
    char *filename = NULL;

    // Alle Sensoren pollen (Nr. 1 ist der "Daylight-Sensor)
    easy_handle = curl_easy_init();
    if( easy_handle )
    {
        struct curl_slist *headers = NULL;

        // Parameter für das Pollen der URL
        setpoll( easy_handle, &headers, &content );

        // dopoll() alloziert Memory für String "filename"
        SensorErrorCode ret = dopoll( easy_handle, &file_written, &filename );
        if( ret != SENSOR_OK ) exitfun( ret, 1 );

        curl_slist_free_all( headers );
        curl_easy_cleanup( easy_handle );
    }
    else exitfun( SENSOR_CURL, 1 );

    // Gelesene Daten (in filename) auf Diskstation kopieren (FTP)
    if( file_written != 0 )
    {
        easy_handle = curl_easy_init();
        if( easy_handle )
        {
            // Parameter für FTP-Filetransfer zur Diskstation
            FILE *file = setftp( easy_handle, filename );
            CURLcode cres = curl_easy_perform( easy_handle );
            fclose( file );

            if( cres != CURLE_OK ) return exitfun( SENSOR_CURL, 1 );
#ifdef DEBUG
            printf( "File %s updated\n", filename );
#endif
            curl_easy_cleanup( easy_handle );
        }
        else exitfun( SENSOR_CURL, 1 );
    }

    free( filename );
}