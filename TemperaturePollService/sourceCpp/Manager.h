
#pragma once

#include <chrono>
#include <vector>
#include "Curl.h"
#include "Credentials.h"
#include "WeatherSensor.h"


using namespace std::chrono;
using namespace std::chrono_literals;


//#define DEBUG                             // debugging enabled
#ifdef  DEBUG
    #define TIMER_REPEAT_TIME   2s          // polling repeat time
#else
    #define TIMER_REPEAT_TIME   15min       // polling repeat time
#endif

#ifdef DEBUG
  #define FILENAME_PREFIX     "SensorTestValues-"
#else
  #define FILENAME_PREFIX     "SensorValues-"
#endif

#define FILENAME_POSTFIX    ".csv"


typedef struct WriteMemoryStruct
{
    int size;
    vector<char> memory;
} WriteMemoryStruct;


typedef duration<int,ratio<1,1>> timespan;


class Manager
{
public:
    // public functions
    Manager();
    ~Manager();

    void executionLoop();        // execution-loop

private:
    void generateFileNames();    // generate local and remote filename
    string manageTime();         // return actual timestamp and provide actual filename for file transfer to NAS
    void transferDataFile();     // FTP-tansfer file to NAS
    string getRawDataString();   // read rawdata string containing data of all Zigbee sensors
    void construct_poll_handle();       // easy handle for Zigbee gateway polling
    void destruct_poll_handle();
    void construct_sendfile_handle();    // easy handle for sending file to NAS
    void destruct_sendfile_handle();
    void setTime( struct tm * );                                // get time stamp from operation system
    string time2string( struct tm );                            // convert time to string
    static size_t write_data( void *, size_t, size_t, void * ); // writefunction for curl

    FILE *fileToSend;                   // file handle of FTP-sent file to NAS
    struct tm oldTime;                  // previous time stamp
    struct tm actTime;                  // actual time stamp
    timespan sleep_time;                // sleeping time span
    string localFileName;               // actual local filename
    string remoteFileName;              // actual remote filename
    WriteMemoryStruct content;          // storage for curl writefunction
    struct curl_slist *poll_headers;    // headers list of easy_handle poll_handle
    CURLM *poll_handle;                 // easy_handle for reading URL
    CURLM *sendfile_handle;             // easy_handle for sending file to NAS
    WeatherSensor *sensor;              // sensor object
};