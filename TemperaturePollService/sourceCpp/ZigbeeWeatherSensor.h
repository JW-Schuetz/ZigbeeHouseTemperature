
#pragma once

#include "ZigbeeSensor.h"


typedef struct WeatherSensorsData
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

  WeatherSensorsData()   // construction
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
} WeatherSensorsData;


typedef vector<WeatherSensorsData> vectorWSData;

// A weather sensor consist of three virtual sensors measuring temperature, pressure and humidity.
// There are maybe more sensors known to the zigbee gateway, for instance a generic
// "daylight sensor". These sensors don't have a battery and are of no interest. 
// They will be ignored in further processing steps.

// handle a weather sensor attached to the zigbee bridge
class ZigbeeWeatherSensor: public ZigbeeSensor
{
public:
    void parseSensorData( string, string );
    void writeDataToFile( string );      // write sensor data into file

private:
    list<string> uniqueSensorNames();   // calculate unique weather sensor names
    vectorWSData parseMeasurementData( string, list<string> );  // provide measured values

    vectorSRData rawDataWeatherSensors;   // raw data of weather sensors
    vectorWSData sensorData;              // parsed data of all sensors
};