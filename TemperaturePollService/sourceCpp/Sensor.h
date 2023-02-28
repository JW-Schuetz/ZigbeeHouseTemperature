
#include "Parser.h"


class Sensor
{
public:
    // public functions
    Sensor();
    ~Sensor();

    static void parseSensorsData( string, string );     // parse rawDataString of all sensors
    static void writeDataToFile( string );              // write sensor data into file

   // public data

private:
    // private functions

    // private data
    static string timeStamp;                                // timeStamp
    static Parser *parser;                                  // object for parsing strings
    static vector<struct PhysicalSensorsData> sensorData;   // parsed data of all sensors
};