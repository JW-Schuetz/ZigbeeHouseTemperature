
#include "WeatherSensor.h"

// initialized storage for static data-members
vector<struct PhysicalSensorsData> WeatherSensor::sensorData {}; // parsed data of all weather sensors


WeatherSensor::WeatherSensor()
{
}


WeatherSensor::~WeatherSensor()
{
}


void WeatherSensor::parseSensorsData( string time )
{
    // count interesting sensors (those with a battery)
    int sensorsOfInterestCount = 0;
    for( auto & iter: rawData )
      if( iter.interesting )
        ++sensorsOfInterestCount;

    // every interesting sensor must consist of 3 "physical" sensors
    // for measuring temperature, pressure and humidity
    if( sensorsOfInterestCount % 3 != 0 )
        throw( string { "Sensor::parseSensorsData sensorsOfInterestCount" } );

    // determine physical sensors names (the names must be unique)
    auto sensorNames = parser->sensorsNames( rawData );

    // plausibility-check: count of physical sensors must be count of interesting
    // sensors divided by 3
    if( (int)sensorNames.size() != sensorsOfInterestCount / 3 )
        throw( string { "Sensor::parseSensorsData physicalSensorsCount" } );

    sensorData = parser->getMeasurementData( sensorNames, rawData, timeStamp );
}