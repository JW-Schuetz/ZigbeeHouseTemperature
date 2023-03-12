
#pragma once

#include <chrono>
#include <vector>
#include "Curl.h"
#include "CurlReadZigbeeBridge.h"
#include "CurlSendFileToNAS.h"
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


typedef duration<int,ratio<1,1>> timespan;


class Manager
{
public:
    // public functions
    Manager();
    ~Manager();

    void executionLoop();               // execution-loop

private:
    void generateFileNames();           // generate local and remote filename
    string manageTime();                // return actual timestamp and provide actual filename for file transfer to NAS
    void transferDataFile();            // FTP-tansfer file to NAS
    void setTime( struct tm * );        // get time stamp from operation system
    string time2string( struct tm );    // convert time to string

    struct tm oldTime;                  // previous time stamp
    struct tm actTime;                  // actual time stamp
    timespan sleepTime;                 // sleeping time span
    string localFileName;               // actual local filename
    string remoteFileName;              // actual remote filename

    CurlReadZigbeeBridge *zigbeeHandle; // pointer to CurlReadZigbeeBridge (for reading Zigbee bridge)
    WeatherSensor *sensor;              // pointer to sensor object
};