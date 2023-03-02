
#include "Parser.h"


class Sensor
{
public:
    // public functions
    Sensor();
    ~Sensor();

    static void parseSensorsData( string );     // parse rawDataString of all sensors
    static void writeDataToFile( string );      // write sensor data into file

   // public data
    static string rawDataString;    // Zigbee's REST-API raw data string containing all sensors and

private:
    // private functions

    // A physical sensor consist of three virtual sensors measuring temperature, pressure and humidity.
    // There are maybe more virtual sensors known to the zigbee gateway, for instance a
    // "daylight sensor". These sensors don't have a battery and are of no interest, 
    // they will be ignored.

    // private data
    static int sensorsOfInterestCount;                    // count of virtual sensors with battery
    static int virtualSensorsCount;                       // count of virtual sensors
    static vector<struct SensorRawData> rawData;          // rawData for each virtual sensor
    static vector<struct PhysicalSensorsData> sensorData; // parsed data of all sensors
    static string timeStamp;                              // polling timeStamp
    static Parser *parser;                                // object for parsing
};