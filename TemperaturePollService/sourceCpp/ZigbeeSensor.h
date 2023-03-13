
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


// handle a sensor attached to the zigbee bridge
class ZigbeeSensor
{
public:
    ZigbeeSensor();
    virtual ~ZigbeeSensor();

protected:
    Parser *parser;       // pointer to parser object
    string timeStamp;     // polling timeStamp
    vectorSRData rawData; // rawData for each sensor

    virtual void parseSensorData( string, string ); // parse sensors rawData
};