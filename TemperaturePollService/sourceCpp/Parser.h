
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Regexp.h"


#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef enum
{
  SENSOR_NOTOK = -1,           // not OK
  SENSOR_OK,                   // all OK
  SENSOR_CURL,                 // curl with problems
  SENSOR_SIG,                  // SIG with problems
  SENSOR_TIMER,                // timer with problems
  SENSOR_PARSE,                // parser with problems
  SENSOR_HW,                   // hardware with problems
  SENSOR_MEMORY,               // no more memory available
} SensorErrorCode;


// handle string parsing using regular expressions
class Parser
{
public:
    Parser();
    ~Parser();

    vector<string> getSensorsRawDataStrings( string );
    string getSensorName( string );
    string getSensorConfig( string );
    string getSensorState( string );
    bool isWeatherSensor( string );
    string getBatteryChargeString( string );
    string getHumidityString( string );
    string getPressureString( string );
    string getTemperatureString( string );
    vector<string> getDateTimeString( string );

private:
    const char *findMatchingCurlyBrace( const char * );
    string getSensorInternalData( string, regex_t );
    string getBatteryCharge( string config, regex_t );
    string getMeasuredValue( string state, regex_t );
    string getLastUpdated( string state, regex_t );
    string getDateTime( string dt, regex_t );

    Regexp *regexp;           // regular expressions, provide compiled form
    regmatch_t pmatch[1];     // regular expressions, regexec() match indices
};