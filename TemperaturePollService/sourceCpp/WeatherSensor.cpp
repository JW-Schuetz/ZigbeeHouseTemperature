
#include "WeatherSensor.h"


list<string> WeatherSensor::uniqueSensorNames()
{
    list<string> sensorsNames = {};   // create an empty list

    for( auto & iter: rawData )
        if( iter.isWeatherSensor != 0 )   // process only isWeatherSensor sensors (with a battery)
          sensorsNames.push_back( iter.name );

    sensorsNames.unique();  // make sensorsNames unique

    return sensorsNames;
}


vector<struct WeatherSensorsData> WeatherSensor::parseMeasurementData( list<string> sensorNames,
    vector<struct SensorRawData> rawData, string timeStamp )
{
  ulong physicalSensorCount = sensorNames.size();

  vector<struct WeatherSensorsData> sensorData( physicalSensorCount ); // create vector of size "physicalSensorCount"

  auto iterName = sensorNames.begin();
  for( auto & iterData: sensorData )
  {
    // sensorname, timestamp
    iterData.sensorname = *(iterName++);
    iterData.owntime = timeStamp;

    for( auto & iter: rawData )
    {
      if( iter.isWeatherSensor )   // process only isWeatherSensor sensors (with a battery)
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


void WeatherSensor::parseSensorData( string time, string rawDataString )
{
    // parse zigbee rawdata string containing all sensors and separate
    // rawdata strings individual for each sensor
    Sensor::parseSensorData( time, rawDataString );  // execute base class functionality

    ulong sensorsOfInterestCount = 0;   // count isWeatherSensor sensors (those with a battery)
    for( auto & iter: rawData )
    {
      iter.isWeatherSensor = parser->isWeatherSensor( string( iter.config ) );
      if( iter.isWeatherSensor ) ++sensorsOfInterestCount;
    }

    // plausibility-check: every isWeatherSensor sensor must consist of 3 "physical"
    // sensors for measuring temperature, pressure and humidity
    if( sensorsOfInterestCount % 3 != 0 )
        throw( string { "WeatherSensor::parseSensorsData sensorsOfInterestCount" } );

    // determine physical sensors names (the names must be unique)
    auto sensorNames = uniqueSensorNames();

    // plausibility-check: count of physical sensors must be count of isWeatherSensor
    // sensors divided by 3
    ulong weatherSensorCount = sensorNames.size();
    if( weatherSensorCount != sensorsOfInterestCount / 3 )
        throw( string { "WeatherSensor::parseSensorsData physicalSensorsCount" } );

    // parse measurement data of all physical sensors using their individual raw data strings
    sensorData = parseMeasurementData( sensorNames, rawData, timeStamp );
}


void WeatherSensor::writeDataToFile( string fileName )
{
  FILE *file = fopen( fileName.c_str(), "a" );
  if( file == NULL ) throw( string { "WeatherSensor::writeDataToFile fopen()" } );

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
  if( ret != 0 ) throw( string { "WeatherSensor::writeDataToFile fclose()" } );
}