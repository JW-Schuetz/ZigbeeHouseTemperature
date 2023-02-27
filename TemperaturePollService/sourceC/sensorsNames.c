
#include "sensors.h"

#ifdef MITSORTIEREN
static int comp( const void *a, const void *b ) 
{
    return( strcmp( (char *)a, (char *)b ) );
}
#endif

char **sensorsNames( int virtSensorCount, struct SensorRawData *rawData,
            int *sensorsNamesCount )
{
    *sensorsNamesCount = 0; // Normierung des Zählers

    // Namen der phys. Sensoren bestimmen (die Namen müssen eindeutig sein)
    char **sensorsNames = NULL;

    for( int sens = 0; sens < virtSensorCount; ++sens )
    {
        // name
        if( rawData[sens].interesting != 0 )   // nur Sensoren mit Auswertung
        {
            char *nameVirtSensor = rawData[sens].name;

            // nameVirtSensor noch nicht in **sensorsNames enthalten?
            int hit = 0;
            for( int n = 0; n < *sensorsNamesCount; ++n )
                if( sensorsNames != NULL && sensorsNames[n] != NULL )
                    if( strcmp( sensorsNames[n], nameVirtSensor ) == 0 ) ++hit;

            if( hit == 0 )
            {
                ++(*sensorsNamesCount);     // neuer SensorName
                int sensorIndex = *sensorsNamesCount - 1;

                // noch nicht enthalten -> Platz reservieren und eintragen
                int size = sizeof(char *);
                sensorsNames = realloc( sensorsNames, (*sensorsNamesCount) * size );
                if( sensorsNames == NULL ) return( NULL );
                memset( &sensorsNames[sensorIndex], 0, size );  // Wechsel: stets 1 Sensor mehr
                sensorsNames[sensorIndex] = nameVirtSensor;
            }
        }
    }

#ifdef MITSORTIEREN
    // Namen alphabetisch sortieren
    qsort( sensorsNames, *sensorsNamesCount, sizeof(char *), comp );
#endif

    return sensorsNames;
}

void destructSensorsNames( char **sensorsNames )
{
    free( sensorsNames );
}