
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Regexp.h"


using std::string;


#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef enum
{
  SENSOR_OK = 0,               // all OK
  SENSOR_CURL,                 // curl with problems
  SENSOR_SIG,                  // SIG with problems
  SENSOR_TIMER,                // timer with problems
  SENSOR_PARSE,                // parser with problems
  SENSOR_HW,                   // hardware with problems
  SENSOR_MEMORY,               // no more memory available
} SensorErrorCode;

struct SensorRawData
{
  bool interesting;   // interesting sensor (with battery) 
  string allData;     // all sensor data
  string name;        // contained sensor name
  string config;      // contained config data
  string state;       // contained state data
};

struct PhysicalSensorsData
{
  string sensorname;
  string batterycharge;
  string humidity;
  string pressure;
  string temperature;
  string sensordate;
  string sensortime;
  string owntime;
  int state;
};


class Parser
{
public:
    // public functions
    Parser();
    ~Parser();

    vector<struct PhysicalSensorsData> getMeasurementData( list<string>,
        vector<struct SensorRawData>, string );
    list<string> sensorsNames( vector<struct SensorRawData> );
    vector<struct SensorRawData> getSensorsRawDataStrings( string );
    string getSensorName( string );
    string getSensorConfig( string );
    string getSensorState( string );
    bool isInterestingSensor( string );

private:
    // private functions
    const char *findMatchingCurlyBrace( const char * );
    string getSensorInternalData( string, regex_t );
    string getBatteryCharge( string config, regex_t );
    string getMeasuredValue( string state, regex_t );
    string getLastUpdated( string state, regex_t );
    string getDateTime( string dt, regex_t );

    // private data
    Regexp *regexp;           // regular expressions: compiled form
    regmatch_t pmatch[1];     // regular expressions: regexec() match indices
};