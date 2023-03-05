
#include "Sensor.h"


// initialized storage for static data-members
string Sensor::zigbeeRawDataString; // Zigbee's REST-API raw data string containing all sensors
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


list<string> Sensor::uniqueSensorNames()
{
    list<string> sensorsNames = {};   // create an empty list

    for( auto & iter: rawData )
        if( iter.interesting != 0 )   // process only interesting sensors (with a battery)
          sensorsNames.push_back( iter.name );

    sensorsNames.unique();  // make sensorsNames unique

    return sensorsNames;
}


vector<struct SensorRawData> Sensor::parseSensorsRawData( string zigbeeRawDataString,
      ulong *sensorsOfInterestCount )
{
    rawData = {};                 // create an empty vector
    *sensorsOfInterestCount = 0;  // normalize counter

    vector<string> rawDataStrings = parser->getSensorsRawDataStrings( zigbeeRawDataString );

    for( auto & iter: rawDataStrings )
    {
      struct SensorRawData raw {};  // create empty data set
      raw.allData = iter;           // write raw data string
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
      iter.interesting = parser->isInterestingSensor( string( iter.config ) );
    
      if( iter.interesting ) ++(*sensorsOfInterestCount);  // increase counter
    }

    return rawData;
}


vector<struct PhysicalSensorsData> Sensor::parseMeasurementData( list<string> sensorNames,
    vector<struct SensorRawData> rawData, string timeStamp )
{
  ulong physicalSensorCount = sensorNames.size();

  vector<struct PhysicalSensorsData> sensorData( physicalSensorCount ); // create vector of size "physicalSensorCount"

  auto iterName = sensorNames.begin();
  for( auto & iterData: sensorData )
  {
    // sensorname, timestamp
    iterData.sensorname = *(iterName++);
    iterData.owntime = timeStamp;

    for( auto & iter: rawData )
    {
      if( iter.interesting )   // process only interesting sensors (with a battery)
      {
        if( iter.name == iterData.sensorname )
        {
          // batterycharge: write only once for every physical sensor
          if( iterData.batterycharge == string() )
            iterData.batterycharge = parser->getBatteryChargeString( iter.config );

          // humidity: write only once for every physical sensor
          if( iterData.humidity == string() )
              iterData.humidity = parser->getHumidityString( iter.state );

          // pressure: write only once for every physical sensor
          if( iterData.pressure == string() )
              iterData.pressure = parser->getPressureString( iter.state );

          // temperature: write only once for every physical sensor
          if( iterData.temperature == string() )
              iterData.temperature = parser->getTemperatureString( iter.state );

          // sensordate, sensortime: write only once for every physical sensor
          if( iterData.sensordate == string() || iterData.sensortime == string() )
          {
            vector<string> datetime = parser->getDateTimeString( iter.state );
            iterData.sensordate = datetime[0];
            iterData.sensortime = datetime[1];
          }
        }
      }
    }

    iterData.state = SENSOR_OK;   // set sensor valid
  }

  return sensorData;
}


void Sensor::parseSensorData( string time )
{
    timeStamp = time; // actual time

    // parse zigbee rawdata string containing all sensors and separate
    // rawdata strings individual for each sensor, count interesting sensors
    ulong sensorsOfInterestCount;   // count interesting sensors (those with a battery)
    rawData = parseSensorsRawData( zigbeeRawDataString, &sensorsOfInterestCount );

    // plausibility-check: every interesting sensor must consist of 3 "physical"
    // sensors for measuring temperature, pressure and humidity
    if( sensorsOfInterestCount % 3 != 0 )
        throw( string { "Sensor::parseSensorsData sensorsOfInterestCount" } );

    // determine physical sensors names (the names must be unique)
    auto sensorNames = uniqueSensorNames();

    // plausibility-check: count of physical sensors must be count of interesting
    // sensors divided by 3
    ulong physicalSensorCount = sensorNames.size();
    if( physicalSensorCount != sensorsOfInterestCount / 3 )
        throw( string { "Sensor::parseSensorsData physicalSensorsCount" } );

    // parse measurement data of all physical sensors using their individual raw data strings
    sensorData = parseMeasurementData( sensorNames, rawData, timeStamp );
}


void Sensor::writeDataToFile( string fileName )
{
  FILE *file = fopen( fileName.c_str(), "a" );
  if( file == NULL ) throw( string { "Sensor::writeDataToFile fopen()" } );

  fprintf( file, "%s", timeStamp.c_str() );
  fprintf( file, ", " );

  auto end = sensorData.end();
  for( auto iter = sensorData.begin(); iter != end; ++iter )
  {
    string str = iter->sensorname;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->batterycharge;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->humidity;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->pressure;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->temperature;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->sensordate;
    fprintf( file, "%s", str.c_str() );
    fprintf( file, ", " );

    str = iter->sensortime;
    fprintf( file, "%s", str.c_str() );

    if( iter != end - 1 ) fprintf( file, ", " );
  }

  fprintf( file, "\n" );

  int ret = fclose( file );
  if( ret != 0 ) throw( string { "Sensor::writeDataToFile fclose()" } );
}