
#include "sensors.h"


void printRegExResults( char *str, int ret, regex_t regex )
{
#ifdef DEBUG
    int sz = 100;
    char errbuff[sz];

    regerror( ret, &regex, errbuff, sz );
    printf( "%s: %s\n", str, errbuff );
#endif
}