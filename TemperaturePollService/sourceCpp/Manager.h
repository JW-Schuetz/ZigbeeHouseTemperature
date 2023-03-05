
#pragma once

#include <chrono>
#include <vector>
#include "Curl.h"
#include "Credentials.h"
#include "Sensor.h"


using namespace std::chrono;


#define DEBUG                             // debugging enabled
#ifdef  DEBUG
    #define TIMER_REPEAT_TIME     2         // polling repeat time [sec]
#else
    #define TIMER_REPEAT_TIME   900         // polling repeat time [sec]
#endif

#ifdef DEBUG
  #define FILENAME_PREFIX     "SensorTestValues-"
#else
  #define FILENAME_PREFIX     "SensorValues-"
#endif

#define FILENAME_POSTFIX    ".csv"


struct WriteMemoryStruct
{
    int size;
    vector<char> memory;
};


class Manager
{
public:
    // public functions
    Manager();
    ~Manager();

    static void executionloop();        // execution-loop

private:
    static void generateFileNames();    // generate local and remote filename
    static string manageTime();         // return actual timestamp and provide actual filename for file transfer to NAS
    static void transferDataFile();     // FTP-tansfer file to NAS
    static string getRawDataString();   // read rawdata string containing data of all Zigbee sensors
    void construct_poll_handle();       // easy handle for Zigbee gateway polling
    void destruct_poll_handle();
    static void construct_sendfile_handle();    // easy handle for sending file to NAS
    static void destruct_sendfile_handle();
    static size_t write_data( void *, size_t, size_t, void * ); // writefunction for curl
    static void setTime( struct tm * );                         // get time stamp from operation system
    static string time2string( struct tm );                     // convert time to string

    static FILE *fileToSend;                                // file handle of FTP-sent file to NAS
    static struct tm oldTime;                               // previous time stamp
    static struct tm actTime;                               // actual time stamp
    static duration<int,ratio<1,1>> sleep_time;             // sleeping time
    static string localFileName;                            // actual local filename
    static string remoteFileName;                           // actual remote filename
    static struct WriteMemoryStruct content;                // storage for curl writefunction
    static struct curl_slist *poll_headers;                 // headers list of easy_handle poll_handle
    static CURLM *poll_handle;                              // easy_handle for reading URL
    static CURLM *sendfile_handle;                          // easy_handle for sending file to NAS
    static Sensor *sensor;                                  // sensor object
};