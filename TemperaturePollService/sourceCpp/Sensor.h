
#pragma once

#include "Parser.h"


typedef struct SensorRawData
{
  bool isWeatherSensor;   // isWeatherSensor sensor (with battery) 
  string allData;         // all sensors raw data
  string name;            // contained sensor name
  string config;          // contained config data
  string state;           // contained state data
} SensorRawData;


typedef vector<SensorRawData> vectorSRData;


class Sensor
{
public:
    Sensor();
    virtual ~Sensor();

protected:
    Parser *parser;     // parser object
    string timeStamp;   // polling timeStamp

    vectorSRData rawData;     // rawData for each sensor

    virtual void parseSensorData( string, string ); // parse sensors rawData
};