
#include "sensors.h"


extern regex_t regex_compiled[];


static int isInterestingSensor( char *config, regex_t regex )
{
  regmatch_t pmatch[1];

  int ret = regexec( &regex, config, ARRAY_SIZE( pmatch ), pmatch, 0 );
  printRegExResults( "regexec batt", ret, regex );

  // Test: virtueller Sensor mit konfigurierter Batterie? Nur solche weiter behandeln.
  if( ret == 0 ) return 1;  // Sensor mit Datenauswerung
  else           return 0;  // Sensor ohne Datenauswerung
}

static char *getSensorName( char *config, regex_t regcomp, char *quantity )
{
    // "name" aus den Rohdaten extrahieren
    regmatch_t pmatch[1];

    int ret = regexec( &regcomp, config, ARRAY_SIZE( pmatch ), pmatch, 0 );
    printRegExResults( quantity, ret, regcomp );

    if( ret != 0 ) return NULL;         // kein Name vorhanden

    config += pmatch[0].rm_eo;              // p: Pointer auf Start Name
    char *q = findNextChar( config, ',' );  // q: Pointer auf Komma hinter Name
    if( q == NULL ) return NULL;            // Parsefehler

    config = config + 1;  // "\"" weglassen
    q      = q - 2;       // Komma und "\"" weglassen

    int size = q - config + 1;
    char *b = (char *)malloc( size + 1 );
    if( b == NULL ) return NULL;
    memset( b, 0, size + 1 );
    memcpy( b, config, size );

    return b;
}

static char *getSensorInternalData( char *s, regex_t regex, char *txt )
{
    // "config" und "state" aus den Rohdaten extrahieren
    regmatch_t pmatch[1];

    int ret = regexec( &regex, s, ARRAY_SIZE( pmatch ), pmatch, 0 );
    printRegExResults( txt, ret, regex );

    if( ret != 0 ) return "";           // keine "internal data" vorhanden

    s += pmatch[0].rm_eo;               // p: Pointer auf Start Sensordaten: "{"
    char *q = findEndOfSensordata( s ); // q: Pointer auf Ende Sensordaten:  "}"
    if( q == NULL ) return NULL;        // Parsefehler

    s = s + 1;  // "{" weglassen
    q = q - 1;  // "}" weglassen

    int size = q - s + 1;
    char *r = (char *)malloc( size + 1 );
    if( r == NULL ) return NULL;
    memset( r, 0, size + 1 );
    memcpy( r, s, size );

    return r;
}

static struct SensorRawData *constructSensorsRawDataStrings( regex_t regex_sensors, 
                        char *toBeMatched, int *count )
{
    // findet alle virtuellen Sensordaten im String "toBeMatched"
    // allociert den Speicher für die Pointer-Liste "struct SensorRawData *"
    // allociert den Speicher für die Elemente-Liste "struct SensorRawData"
    // allociert den Speicher für den String "allData"

  //------------------------------------------------------------------
  //   ---------------------                --------------------
  //  |    SensorRawData    |   Elemente:  |   int  interesting |
  //  |         0           |              |   char *allData    |
  //   ---------------------               |   char *name       |
  //            .                          |   char *config     |
  //            .                          |   char *state      |
  //            .                           --------------------
  //   ---------------------
  //  |    SensorRawData    |
  //  |    sensorIndex      |
  //   ---------------------
  //-------------------------------------------------------------------

    *count = 0; // Normierung des Zählers

    char *q;
    char *p = toBeMatched;

    // Platz für Array, wird später mit realloc() vergrössert
    struct SensorRawData *rawData = NULL;

    regmatch_t pmatch[1];

    // Schleife bis kein Sensor mehr gefunden wird
    while( 1 )
    {
      int ret = regexec( &regex_sensors, p, ARRAY_SIZE( pmatch ), pmatch, 0 );
      printRegExResults( "regexec sensors", ret, regex_sensors );

      if( ret != 0 ) break; // kein weiterer Sensor vorhanden: raus

      ++(*count);       // neuer Sensor gefunden

      // Pointerarray für den neuen Sensor erweitern
      int size = sizeof(struct SensorRawData);
      rawData = (struct SensorRawData *)realloc( rawData, (*count) * size );
      if( rawData == NULL ) return( NULL );
      memset( &rawData[*count - 1], 0, size );  // Wechsel: stets 1 Sensor mehr

      p += pmatch[0].rm_eo;           // p: Pointer auf Start der Sensordaten: "{"
      q = findEndOfSensordata( p );   // q: Pointer auf Ende der Sensordaten:  "}"
      if( q == NULL ) return( NULL ); // Parsefehler

      p = p + 1;  // "{" weglassen
      q = q - 1;  // "}" weglassen

      size = q - p + 1;
      char *s = (char *)malloc( size + 1 );
      if( s == NULL ) return( NULL );
      memset( s, 0, size + 1 );
      memcpy( s, p, size );

      // Ergebnis des Sensors eintragen
      rawData[*count - 1].allData = s;
    }

    return rawData;
}

struct SensorRawData *sensorsRawDataStrings( char *parsePtr, int *virtSensorCount, 
                        int *sensorsOfInterestCount )
{
  // Rohdatenstrings aller virtuellen Sensoren finden und abspeichern
  struct SensorRawData *rawData = constructSensorsRawDataStrings( regex_compiled[0], parsePtr,
                                      virtSensorCount );
  if( rawData == NULL ) return NULL;

  *sensorsOfInterestCount = 0;    // Anzahl virtueller Sensoren, die ausgewertet werden sollen
  for( int sens = 0; sens < *virtSensorCount; ++sens )
  {
    // name
    char *str = getSensorName( rawData[sens].allData, regex_compiled[1], "regexec name" );
    if( str == NULL ) return NULL;  // Fehler beim Parsen
    rawData[sens].name = str;

    // config
    str = getSensorInternalData( rawData[sens].allData, regex_compiled[2], "regexec config" );
    if( str == NULL ) return NULL;  // Fehler beim Parsen
    rawData[sens].config = str;

    // state
    str = getSensorInternalData( rawData[sens].allData, regex_compiled[3], "regexec state" );
    if( str == NULL ) return NULL;  // Fehler beim Parsen
    rawData[sens].state = str;

    // Test: mit/ohne Datenauswertung
    rawData[sens].interesting = 0;   // default: Sensor ohne Datenauswerung
    if( isInterestingSensor( rawData[sens].config, regex_compiled[4] ) )
    {
      rawData[sens].interesting = 1;   // Sensor mit Datenauswerung
      ++(*sensorsOfInterestCount);
    }
  }

  return rawData;
}

void destructSensorsRawDataStrings( struct SensorRawData *rawData, int count )
{
  for( int n = 0; n < count; ++n )
  {
    free( rawData[n].allData );
    free( rawData[n].name );
    free( rawData[n].config );
    free( rawData[n].state );
  }

  free( rawData ); // rawData freigeben, die Strings in den Elementen allData müssen später freigegeben werden
}