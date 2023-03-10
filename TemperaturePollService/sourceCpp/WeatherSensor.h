
#pragma once

#include "Sensor.h"


struct WeatherSensorsData
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
};


class WeatherSensor: public Sensor
{
public:
    void parseSensorData( string, string );
    void writeDataToFile( string );      // write sensor data into file

private:
    list<string> uniqueSensorNames();   // calculate unique weather sensor names
    vector<struct WeatherSensorsData> parseMeasurementData( list<string>,
        vector<struct SensorRawData>, string );

    // A weather sensor consist of three virtual sensors measuring temperature, pressure and humidity.
    // There are maybe more sensors known to the zigbee gateway, for instance a generic
    // "daylight sensor". These sensors don't have a battery and are of no interest, 
    // they will be ignored.

    vector<struct WeatherSensorsData> sensorData; // parsed data of all sensors
};