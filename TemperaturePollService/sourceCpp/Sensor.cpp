
#include "Sensor.h"


// initialized storage for static data-members
int Sensor::sensorsOfInterestCount {};  // count of sensors with battery
int Sensor::virtualSensorsCount {};     // count of virtual sensors
string Sensor::rawDataString;           // Zigbee's REST-API raw data string containing all sensors
vector<struct SensorRawData> Sensor::rawData {};          // rawData for each virtual sensor
vector<struct PhysicalSensorsData> Sensor::sensorData {}; // parsed data of all sensors
string Sensor::timeStamp {};                              // timeStamp
Parser *Sensor::parser {};                                // parser object


Sensor::Sensor()
{
  parser = new Parser();
}


Sensor::~Sensor()
{
    delete parser;
}


void Sensor::parseSensorsData( string time )
{
    timeStamp = time;

    // parse rawdata string containing all sensors and separate individual
    // rawdata strings for each sensor
    rawData = parser->getSensorsRawDataStrings( rawDataString );

    // parse individual rawdata string for each sensor for getting
    // sensors name, config- and state-strings and test whether 
    // it is interesting for us (has an battery)
    for( auto & iter: rawData )
    {
      iter.name = parser->getSensorName( string( iter.allData ) );
      iter.config = parser->getSensorConfig( string( iter.allData ) );
      iter.state = parser->getSensorState( string( iter.allData ) );
      iter.interesting = parser->isInterestingSensor( string( iter.config ) );
    }

    // count interesting sensors (those with a battery)
    sensorsOfInterestCount = 0;
    for( auto & iter: rawData )
      if( iter.interesting )
        ++sensorsOfInterestCount;

    // every interesting sensor must consist of 3 "physical" sensors
    // for measuring temperature, pressure and humidity
    if( sensorsOfInterestCount % 3 != 0 )
        throw( string { "Sensor::parseSensorsData sensorsOfInterestCount" }  );

    // determine the sensors names (the names must be unique)
    int physicalSensorsCount = 0;
    auto sensorNames = parser->sensorsNames( rawData, &physicalSensorsCount );

    // plausibility-check: count of physical sensors must be count of interesting sensors div. by 3
    if( physicalSensorsCount != sensorsOfInterestCount / 3 )
        throw( string { "Sensor::parseSensorsData physicalSensorsCount" }  );

    sensorData = parser->getMeasurementData( sensorNames, rawData, timeStamp );
}


void Sensor::writeDataToFile( string fileName )
{
  FILE *file = fopen( fileName.c_str(), "a" );
  if( file == NULL ) throw( string { "Sensor::writeDataToFile fopen" }  );

  fprintf( file, "%s", timeStamp.c_str() );
  fprintf( file, ", " );

  string str;
  for( size_t i = 0; i < sensorData.size(); ++i )
  {
    str = sensorData[i].sensorname;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].batterycharge;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].humidity;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].pressure;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].temperature;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].sensordate;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = sensorData[i].sensortime;
    fprintf( file, "%s", str.c_str() );

    if( i != sensorData.size() - 1 ) fprintf( file, ", " );
  }

  fprintf( file, "\n" );

  int ret = fclose( file );
  if( ret != 0 ) throw( string { "Sensor::writeDataToFile fclose" }  );
}