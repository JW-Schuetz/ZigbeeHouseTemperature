
#pragma once

#include <string>
#include <vector>
#include "Regexp.h"


using std::string;


#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef enum
{
  SENSOR_OK = 0,               // alles OK
  SENSOR_CURL,                 // Curl hat Probleme
  SENSOR_SIG,                  // SIG hat Probleme
  SENSOR_TIMER,                // Timer hat Problem
  SENSOR_PARSE,                // Parser hat Problem
  SENSOR_HW,                   // Hardware hat Probleme
  SENSOR_MEMORY,               // kein Speicher mehr da
} SensorErrorCode;

struct SensorRawData
{
  int interesting;    // Sensor mit Datenauswertung 
  string allData;     // kompletter Sensordaten-String
  string name;        // darin enthaltener Sensorname
  string config;      // darin enthaltener Configdaten-String
  string state;       // darin enthaltener Statedaten-String
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

    vector<struct PhysicalSensorsData> getMeasurementData( int, int,
      vector<string>, vector<struct SensorRawData>, string );
    vector<string> sensorsNames( int, vector<struct SensorRawData>, int * );
    vector<struct SensorRawData> getSensorsRawDataStrings( string );
    string getSensorName( string );
    string getSensorConfig( string );
    string getSensorState( string );
    int isInterestingSensor( string );

private:
    // private functions
    const char *findMatchingCurlyBrace( const char * );
    const char *findLastDigitSigned( const char *p );
    const char *findNextChar( const char *, char );
    string getSensorInternalData( string, regex_t );
    string getBatteryCharge( string config, regex_t );
    string getMeasuredValue( string state, regex_t );
    string getLastUpdated( string state, regex_t );
    string getDateTime( string dt, regex_t );

    // private data
    Regexp *regexp;          // compiled regular expressions
};