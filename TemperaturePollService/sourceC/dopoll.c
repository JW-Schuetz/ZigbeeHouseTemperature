
#include "sensors.h"


extern struct MemoryStruct content;


SensorErrorCode dopoll( CURLM *easy_handle, int *file_written, char **filename )
{
  *filename = NULL;

  content.memory = (char *)malloc( 1 );    // Memory wächst durch realloc in Funktion write_data()
  if( content.memory == NULL ) return SENSOR_PARSE;
  content.size = 0;                        // z.Z. noch keine Daten

  CURLcode cres = curl_easy_perform( easy_handle );
  if( cres != CURLE_OK ) return SENSOR_CURL;

  int sensorsCount;
  struct PhysicalSensorsData *sensorsData = parseVirtualSensors( content.memory, &sensorsCount );
  if( sensorsData == NULL ) return SENSOR_MEMORY;
  if( sensorsCount == 0 ) return SENSOR_HW;

  free( content.memory ); // content Speicher wieder freigeben

  // Zeitmarke des 1. Sensors nehmen, String duplizieren wg. Modifikation durch strtok()
  char *ownTime = strdup( sensorsData[0].owntime );
  char *datum = strtok( ownTime, " " );

  // Filename generieren, Speicher wird in signalhandler() wieder freigegeben.
  int size = strlen( FILENAME_PREFIX ) + strlen( datum ) + strlen( FILENAME_POSTFIX ) + 1;
  *filename = (char *)malloc( size );
  if( *filename == NULL ) return SENSOR_PARSE;
  memset( *filename, 0, size );

  *filename = strcat( *filename, FILENAME_PREFIX );
  *filename = strcat( *filename, datum );
  *filename = strcat( *filename, FILENAME_POSTFIX );

  free( ownTime );

  FILE *file = fopen( *filename, "a" );

  fwrite( sensorsData[0].owntime, strlen( sensorsData[0].owntime ), 1, file );
  fwrite( ", ", 2, 1, file );

  char *str;
  for( int sens = 0; sens < sensorsCount; ++sens )
  {
    str = sensorsData[sens].sensorname;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].batterycharge;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].humidity;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].pressure;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].temperature;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].sensordate;
    fwrite( str, strlen( str ), 1, file );
    fwrite( ", ", 2, 1, file );

    str = sensorsData[sens].sensortime;
    fwrite( str, strlen( str ), 1, file );
    if( sens != (sensorsCount - 1) )
      fwrite( ", ", 2, 1, file );
  }

  fwrite( "\n", strlen( "\n" ), 1, file );

  fclose( file );

  *file_written = 1;  // Merker: neuer Eintrag im File

  destructSensorsData( sensorsData, sensorsCount );

  return SENSOR_OK;
}