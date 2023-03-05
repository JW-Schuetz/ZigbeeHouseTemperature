
#pragma once

#include "Parser.h"


struct SensorRawData
{
  bool interesting;   // interesting sensor (with battery) 
  string allData;     // all sensors raw data
  string name;        // contained sensor name
  string config;      // contained config data
  string state;       // contained state data
};


struct PhysicalSensorsData
{
public:
  string sensorname;
  string batterycharge;
  string humidity;
  string pressure;
  string temperature;
  string sensordate;
  string sensortime;
  string owntime;
  int state;

  PhysicalSensorsData()   // construction
  {
    sensorname = {};
    batterycharge = {};
    humidity = {};
    pressure = {};
    temperature = {};
    sensordate = {};
    sensortime = {};
    owntime = {};
    state = SENSOR_NOTOK;
  }
};


class Sensor
{
public:
    Sensor();
    ~Sensor();

    static void parseSensorData( string );      // parse rawDataString of all sensors
    static void writeDataToFile( string );      // write sensor data into file

    static string zigbeeRawDataString;          // Zigbee's REST-API raw data string containing all sensors

private:
    static list<string> uniqueSensorNames();   // calculate unique sensor names
    static vector<struct SensorRawData> parseSensorsRawData( string, ulong * );
    static vector<struct PhysicalSensorsData> parseMeasurementData( list<string>,
      vector<struct SensorRawData>, string );

    // A physical sensor consist of three virtual sensors measuring temperature, pressure and humidity.
    // There are maybe more virtual sensors known to the zigbee gateway, for instance a generic
    // "daylight sensor". These sensors don't have a battery and are of no interest for me, 
    // they will be ignored.

    static vector<struct SensorRawData> rawData;          // rawData for each virtual sensor
    static vector<struct PhysicalSensorsData> sensorData; // parsed data of all sensors
    static string timeStamp;                              // polling timeStamp
    static Parser *parser;                                // parser object
};