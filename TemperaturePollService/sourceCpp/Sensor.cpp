
#include "Sensor.h"


Sensor::Sensor()
{
  parser = new Parser();
}


Sensor::~Sensor()
{
    delete parser;
}


void Sensor::parseSensorData( string time, string zigbeeRawDataString )
{
    timeStamp = time;   // actual time

    vector<string> rawDataStrings = parser->getSensorsRawDataStrings( zigbeeRawDataString );

    rawData = {};                     // create an empty vector

    for( auto & iter: rawDataStrings )
    {
      struct SensorRawData raw {};    // create empty data set
      raw.allData = iter;             // write raw data string

      rawData.push_back( raw );
    }

    // parse individual rawdata string for each sensor for getting
    // sensors name, config- and state-strings and test whether 
    // sensor is interesting for us (that is, it contains a battery)
    for( auto & iter: rawData )
    {
      iter.name = parser->getSensorName( string( iter.allData ) );
      iter.config = parser->getSensorConfig( string( iter.allData ) );
      iter.state = parser->getSensorState( string( iter.allData ) );
      iter.isWeatherSensor = parser->isWeatherSensor( string( iter.config ) );
    }
}