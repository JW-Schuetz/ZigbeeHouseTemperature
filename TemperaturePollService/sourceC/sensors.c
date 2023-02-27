
#include "sensors.h"


int main( void )
{
  // Compile regular expressions
  constructRegexp();

  // Curl initialisieren
  SensorErrorCode ret = curl_global_init( CURL_GLOBAL_ALL );
  if( ret != CURLE_OK ) exitfun( ret, 1 );

  // Timer initialisieren
  ret = inittimer();
  if( ret != SENSOR_OK ) exitfun( ret, 1 );

  // Timer starten
  ret = starttimer();
  if( ret != SENSOR_OK ) exitfun( ret, 1 );

  // Warteschleife
  while( 1 ) sleep( 5 * TIMER_REPEAT_TIME );
 
  // der nachfolgende Code wird nie erreicht
  curl_global_cleanup();

  // Free compiled regular expressions
  destructRegexp();

  return SENSOR_CURL;
}