
#pragma once

#include "Sensor.h"


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


// A weather sensor consist of three virtual sensors measuring temperature, pressure and humidity.

class WeatherSensor: Sensor
{
public:
    WeatherSensor();
    ~WeatherSensor();

protected:
    void parseSensorsData( string time );

private:
    static vector<struct PhysicalSensorsData> sensorData; // parsed data of all weather sensors
};