
#pragma once

#include "Parser.h"


class Sensor
{
public:
    Sensor();
    ~Sensor();

    static void parseSensorsData( string );     // parse rawDataString of all sensors
    static void writeDataToFile( string );      // write sensor data into file

    static string rawDataString;    // Zigbee's REST-API raw data string containing all sensors

private:
    // A physical sensor consist of three virtual sensors measuring temperature, pressure and humidity.
    // There are maybe more virtual sensors known to the zigbee gateway, for instance a generic
    // "daylight sensor". These sensors don't have a battery and are of no interest for us, 
    // they will be ignored.

    static vector<struct SensorRawData> rawData;          // rawData for each virtual sensor
    static vector<struct PhysicalSensorsData> sensorData; // parsed data of all sensors
    static string timeStamp;                              // polling timeStamp
    static Parser *parser;                                // parser object
};