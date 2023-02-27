
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <curl/curl.h>
#include "credentials.h"    // enthält Defines für: URL, DISKSTATION und FTPCREDENTIALS


#define DEBUG                               // debugging enabled
#ifdef  DEBUG
    #define TIMER_REPEAT_TIME     2         // polling repeat time [sec]
#else
    #define TIMER_REPEAT_TIME   900         // polling repeat time [sec]
#endif

#ifdef DEBUG
  #define FILENAME_PREFIX     "SensorTestValues-"
#else
  #define FILENAME_PREFIX     "SensorValues-"
#endif

#define FILENAME_POSTFIX    ".csv"

typedef enum
{
  SENSOR_OK = 0,               // alles OK
  SENSOR_CURL,                 // Curl hat Probleme
  SENSOR_SIG,                  // SIG hat Probleme
  SENSOR_TIMER,                // Timer hat Problem
  SENSOR_PARSE,                // Parser hat Problem
  SENSOR_HW,                   // Hardware hat Probleme
  SENSOR_MEMORY,               // kein Speicher mehr da
} SensorErrorCode;

struct MemoryStruct
{
  char *memory;
  size_t size;
};

struct SensorRawData
{
  int interesting;    // Sensor mit Datenauswertung 
  char *allData;      // kompletter Sensordaten-String
  char *name;         // darin enthaltener Sensorname
  char *config;       // darin enthaltener Configdaten-String
  char *state;        // darin enthaltener Statedaten-String
};

struct PhysicalSensorsData
{
  char *sensorname;
  char *batterycharge;
  char *humidity;
  char *pressure;
  char *temperature;
  char *sensordate;
  char *sensortime;
  char *owntime;
  int state;
};

SensorErrorCode inittimer();
SensorErrorCode starttimer();
size_t write_data( void *, size_t, size_t, void * );
void setpoll( CURLM *, struct curl_slist **, struct MemoryStruct * );
FILE *setftp( CURLM *, char * );
SensorErrorCode doftp( CURLM * );
SensorErrorCode dopoll( CURLM *, int *, char ** );
void constructRegexp();
void destructRegexp();
char *findNextChar( char *, char );
char *findLastDigitSigned( unsigned char * );
struct SensorRawData *sensorsRawDataStrings( char *, int *, int * );
void destructSensorsRawDataStrings( struct SensorRawData *, int );
char **sensorsNames( int, struct SensorRawData *, int * );
void destructSensorsNames( char ** );
void destructSensorsData( struct PhysicalSensorsData *, int );
struct PhysicalSensorsData *parseVirtualSensors( char *, int * );
char *findEndOfSensordata( char * );
void printRegExResults( char *, int, regex_t );
void exitfun( SensorErrorCode, int );

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))