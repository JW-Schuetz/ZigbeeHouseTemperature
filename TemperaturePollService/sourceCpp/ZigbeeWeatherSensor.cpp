
#include "ZigbeeWeatherSensor.h"


void ZigbeeWeatherSensor::parseSensorData( string time, string rawDataString )
{
    // parse zigbee rawdata string containing all sensors and separate
    // rawdata strings individual for each sensor
    ZigbeeSensor::parseSensorData( time, rawDataString );  // execute base class functionality

    rawDataWeatherSensors = {};  // create an empty vector

    for( auto & iter: rawData )
    {
      iter.isWeatherSensor = parser->isWeatherSensor( string( iter.config ) );
      if( iter.isWeatherSensor ) rawDataWeatherSensors.push_back( iter );
    }

    // plausibility-check: every weather sensor must consist of 3 "physical"
    // sensors for measuring temperature, pressure and humidity
    size_t physicalSensorsCount = rawDataWeatherSensors.size();
    if( physicalSensorsCount % 3 != 0 )
        throw( string { "ZigbeeWeatherSensor::parseSensor()sData physicalSensorsCount" } );

    // determine physical sensors names (the names must be unique)
    auto sensorNames = uniqueSensorNames();

    // plausibility-check: count of physical sensors must be count of weather 
    // sensors divided by 3
    ulong weatherSensorCount = sensorNames.size();
    if( weatherSensorCount != physicalSensorsCount / 3 )
        throw( string { "ZigbeeWeatherSensor::parseSensorsData weatherSensorCount" } );

    // parse measurement data of all physical sensors using their individual raw data strings
    sensorData = parseMeasurementData( timeStamp, sensorNames );
}


list<string> ZigbeeWeatherSensor::uniqueSensorNames()
{
    list<string> sensorsNames = {};   // create an empty list

    for( auto & iter: rawDataWeatherSensors ) sensorsNames.push_back( iter.name );

    sensorsNames.unique();  // make sensorsNames unique

    return sensorsNames;
}


vectorWSData ZigbeeWeatherSensor::parseMeasurementData( string timeStamp , list<string> sensorNames )
{
  ulong physicalSensorCount = sensorNames.size();
  vectorWSData sensorData( physicalSensorCount ); // create vector of size "physicalSensorCount"

  auto iterName = sensorNames.begin();
  for( auto & iterData: sensorData )
  {
    // sensorname, timestamp
    iterData.sensorname = *(iterName++);
    iterData.owntime = timeStamp;

    for( auto & iter: rawDataWeatherSensors )
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

    iterData.state = SENSOR_OK;   // set sensor valid
  }

  return sensorData;
}


void ZigbeeWeatherSensor::writeDataToFile( string fileName )
{
  FILE *file = fopen( fileName.c_str(), "a" );
  if( file == NULL ) throw( string { "ZigbeeWeatherSensor::writeDataToFile fopen()" } );

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
  if( ret != 0 ) throw( string { "ZigbeeWeatherSensor::writeDataToFile fclose()" } );
}