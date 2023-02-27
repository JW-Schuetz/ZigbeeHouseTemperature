
#include <stdio.h>


char *findEndOfSensordata( char *p )
{
    char c = *p;                  // erstes Zeichen lesen

    if( c == 0 ) return NULL;     // Stringende erreicht
    if( c != '{' ) return NULL;   // p zeigt nicht auf Start von Sensordaten

    int counter = 0;              // Anzahl offener KLammerpaare {}

    while( 1 )
    {
      c = *(++p);                 // nächstes Zeichen lesen

      if( c == 0 ) return NULL;   // Stringende erreicht
      if( c == '}' )
      {
        if( counter == 0 ) break; // Ende der Sensordaten erreicht, p zeigt auf "}"
        --counter;                // ein Klammerpaar weniger
      }

      if( c == '{' ) ++counter;   // ein Klammerpaar mehr
    }

    return p;
}