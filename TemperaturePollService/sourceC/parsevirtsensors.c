
#include <time.h>
#include <ctype.h>
#include "sensors.h"


extern regex_t regex_compiled[];


char *findNextChar( char *p, char s )
{
    char c = *p;                    // erstes Zeichen lesen

    if( c == 0 ) return NULL;       // Stringende erreicht

    while( 1 )
    {
      c = *(++p);                   // nächstes Zeichen lesen

      if( c == 0 ) return NULL;     // Stringende erreicht
      if( c == s ) break;
    }

    return p;
}

char *findLastDigitSigned( unsigned char *p )
{
    char c = *p;                            // erstes Zeichen lesen

    if( c == 0 ) return NULL;               // Stringende unerwartet erreicht

    if( isdigit( c ) == 0 && ( c != '-' ) ) // 1. Zeichen darf '-'' sein
      return NULL;    // leerer Name

    while( 1 )
    {
      c = *(++p);                           // nächstes Zeichen lesen

      if( isdigit( c ) == 0 ) break;
      if( c == 0 ) return NULL;             // Stringende unerwartet erreicht
    }

    return p - 1;                           // Pointer zurückpositionieren
}

static char *getBatteryCharge( char *config, regex_t regcomp, char *quantity )
{
  regmatch_t pmatch[1];

  int ret = regexec( &regcomp, config, ARRAY_SIZE( pmatch ), pmatch, 0 );
  printRegExResults( quantity, ret, regcomp );

  // die Batterie ist sicher vorhanden, da Sensor "interesting" ist
  config += pmatch[0].rm_eo;          // p: Pointer auf Start Batterieladung
  char *q = findLastDigitSigned( config );  // q: Pointer auf letzte Zahl Batterieladung
  if( q == NULL ) return NULL;        // Parsefehler

  int size = q - config + 1;
  char *r = (char *)malloc( size + 1 );
  if( r == NULL ) return NULL;
  memset( r, 0, size + 1 );
  memcpy( r, config , size );

  return r;
}

static char *getMeasuredValue( char *state, regex_t regcomp, char *quantity )
{
  regmatch_t pmatch[1];

  int ret = regexec( &regcomp, state, ARRAY_SIZE( pmatch ), pmatch, 0 );
  printRegExResults( quantity, ret, regcomp );

  if( ret == 0 )
  {
    state += pmatch[0].rm_eo;         // p: Pointer auf Start Messwert
    char *q = findLastDigitSigned( state ); // q: Pointer auf letzte Zahl Messwert
    if( q == NULL ) return NULL;      // Parsefehler

    int size = q - state + 1;
    char *r = (char *)malloc( size + 1 );
    if( r == NULL ) return NULL;
    memset( r, 0, size + 1 );
    memcpy( r, state , size );

    return r;
  }

  return NULL;
}

static char *getLastUpdated( char *state, regex_t regcomp, char *quantity )
{
  regmatch_t pmatch[1];

  int ret = regexec( &regcomp, state, ARRAY_SIZE( pmatch ), pmatch, 0 );
  printRegExResults( quantity, ret, regcomp );

  if( ret == 0 )
  {
    state += pmatch[0].rm_eo;               // p: Pointer auf Start Zeitmarke
    char *q = findNextChar( state, '\"' );  // q: Pointer auf letztes '\"" Zeitmarke
    if( q == NULL ) return NULL;            // Parsefehler

    state = state + 1;  // Anfang: Zeichen '\"" entfernen
    q     = q - 1;      // Ende:   Zeichen '\"" entfernen

    int size = q - state + 1;
    char *r = (char *)malloc( size + 1 );
    if( r == NULL ) return NULL;
    memset( r, 0, size + 1 );
    memcpy( r, state , size );

    return r;
  }

  return NULL;
}

static char *getDateTime( char *dt, regex_t regcomp, char *quantity )
{
  regmatch_t pmatch[1];

  int ret = regexec( &regcomp, dt, ARRAY_SIZE( pmatch ), pmatch, 0 );
  printRegExResults( quantity, ret, regcomp );

  if( ret == 0 )
  {
    char *p = dt + pmatch[0].rm_so;  // p: Pointer auf Start Datum
    char *q = dt + pmatch[0].rm_eo;  // p: Pointer auf Ende  Datum

    q = q - 1;
  
    int size = q - p + 1;
    char *r = (char *)malloc( size + 1 );
    if( r == NULL ) return NULL;
    memset( r, 0, size + 1 );
    memcpy( r, p , size );

    return r;
  }

  return NULL;
}

static char *getOwnTime()
{
#define NALLOC 50
  struct tm *tbuff = malloc( sizeof( struct tm ) );

  char * cbuff = malloc( NALLOC );
  time_t t = time( NULL );
  struct tm tm = *localtime_r( &t, tbuff );
  snprintf( cbuff, NALLOC, "%02d.%02d.%04d %02d:%02d:%02d", 
    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 
    tm.tm_hour, tm.tm_min, tm.tm_sec );

  free( tbuff );

  return cbuff;
}

struct PhysicalSensorsData *parseVirtualSensors( char *parsePtr, int *physSensorsCount )
{
  *physSensorsCount = 0; // Normierung des Zählers

  int virtSensorsCount = 0;
  int sensorsOfInterestCount = 0;
  struct SensorRawData *rawData = sensorsRawDataStrings( parsePtr, &virtSensorsCount,
                                    &sensorsOfInterestCount );

  // jeder der hier benutzten phys. Sensoren muss aus 3 virt. Sensoren bestehen:
  //   für Temperatur/Druck/Feuchte
  if( sensorsOfInterestCount % 3 != 0 ) return NULL;

  *physSensorsCount = sensorsOfInterestCount / 3;

  // Namen der phys. Sensoren bestimmen (die Namen müssen eindeutig sein)
  int sensorsNamesCount = 0;
  char **sensorNames = sensorsNames( virtSensorsCount, rawData, &sensorsNamesCount );  // die Anzahl der Sensornamen muss mit der Anzahl der phys. Sensoren matchen

  // Plausibilitäts-Prüfung
  if( sensorsNamesCount != *physSensorsCount ) return NULL;

  // Speicherplatz allocieren
  int size = (*physSensorsCount) * sizeof(struct PhysicalSensorsData);
  struct PhysicalSensorsData *sensordata = (struct PhysicalSensorsData *)malloc( size );
  if( sensordata == NULL ) return NULL;
  memset( &sensordata[0], 0, size );

  regmatch_t pmatch[1];

  // Zeitmarke, für alle Sensoren gleich
  char *ownTime = getOwnTime();

  // für jeden physikalischen Sensor die Rohdatenstrings parsen
  for( int sens = 0; sens < *physSensorsCount; ++sens )
  {
    // Sensornamen übernehmen
    sensordata[sens].sensorname = strdup( sensorNames[sens] );

    // Zeitmarke
    sensordata[sens].owntime = strdup( ownTime );

    // Daten aller virtuellen Sensoren mit gleichem Namen akkumulieren
    for( int n = 0; n < virtSensorsCount; ++n )
    {
      if( rawData[n].interesting != 0 )   // nur Sensoren mit Auswertung
      {
        if( strcmp( sensordata[sens].sensorname, rawData[n].name ) == 0 )
        {
          // batterycharge: nur ein mal eintragen pro phys. Sensor
          if( sensordata[sens].batterycharge == NULL )
          {
            char *b = getBatteryCharge( rawData[n].config, regex_compiled[4], 
                        "regexec batterycharge" );
            sensordata[sens].batterycharge = b;
          }

          // humidity: nur ein mal eintragen pro phys. Sensor
          if( sensordata[sens].humidity == NULL )
          {
            char *r = getMeasuredValue( rawData[n].state, regex_compiled[5], "regexec humidity" );
            if( r != NULL ) sensordata[sens].humidity = r;
          }

          // pressure: nur ein mal eintragen pro phys. Sensor
          if( sensordata[sens].pressure == NULL )
          {
            char *p = getMeasuredValue( rawData[n].state, regex_compiled[6], "regexec pressure" );
            if( p != NULL ) sensordata[sens].pressure = p;
          }

          // temperature: nur ein mal eintragen pro phys. Sensor
          if( sensordata[sens].temperature == NULL )
          {
            char *t = getMeasuredValue( rawData[n].state, regex_compiled[7], "regexec temperature" );
            if( t != NULL ) sensordata[sens].temperature = t;
          }

          // sensordate, sensortime: nur ein mal eintragen pro phys. Sensor
          if( sensordata[sens].sensordate == NULL )
          {
            // lastupdated
            char *lastupdated = getLastUpdated( rawData[n].state, regex_compiled[8], "regexec lastupdated" );
            if( lastupdated == NULL ) return NULL;
          
            // date
            char *d = getDateTime( lastupdated, regex_compiled[9], "regexec date" );
            if( d != NULL ) sensordata[sens].sensordate = d;

            // time
            char *t = getDateTime( lastupdated, regex_compiled[10], "regexec time" );
            if( t != NULL ) sensordata[sens].sensortime = t;

            free( lastupdated );
          }
        }
      }
    }

    sensordata[sens].state = SENSOR_OK;   // gültig setzen
  }

  // Allocierten Speicher wieder freigeben
  free( ownTime );
  destructSensorsNames( sensorNames );
  destructSensorsRawDataStrings( rawData, virtSensorsCount );

  return sensordata;
}

void destructSensorsData( struct PhysicalSensorsData *data, int sensorsCount )
{
  for( int sens = 0; sens < sensorsCount; ++sens )
  {
    free( data[sens].owntime );
    free( data[sens].batterycharge );
    free( data[sens].sensorname );
    free( data[sens].sensordate );
    free( data[sens].sensortime );
    free( data[sens].humidity );
    free( data[sens].pressure );
    free( data[sens].temperature );
  }

  free( data );
}