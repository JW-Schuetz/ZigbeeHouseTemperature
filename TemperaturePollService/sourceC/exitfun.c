
#include "sensors.h"


void exitfun( SensorErrorCode state, int doexit )
{
  extern int errno;

  int ret = 1;
  char *errstr;
  switch( state )
  {
    case SENSOR_OK:
      errstr = "Programm erfogreich beendet";
      ret = 0;
      break;
    case SENSOR_PARSE:
      errstr = "Laufzeitfehler: Fehler beim Parsen";
      break;
    case SENSOR_CURL:
      errstr = "Laufzeitfehler: Curl hat ein Problem";
      break;
    case SENSOR_SIG:
      errstr = "Laufzeitfehler: Signal hat ein Problem";
      break;
    case SENSOR_TIMER:
      errstr = "Laufzeitfehler: Timer hat ein Problem";
      break;
  }

  fprintf( stderr, "%s\n\n", errstr );
  if( doexit != 0 ) exit( ret );
}