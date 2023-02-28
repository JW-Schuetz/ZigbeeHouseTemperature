
#include "Regexp.h"
#include "Manager.h"


Regexp::Regexp()
{
  regex_compiled = (regex_t *)malloc( N * sizeof( regex_t ) );

  for( int n = 0; n < N; ++n )
  {
#ifdef DEBUG
    int ret = regcomp( &regex_compiled[n], regexp[n].c_str(), REG_EXTENDED );
    printResults( regexp[n], ret, regex_compiled[n] );
#else
    regcomp( &regex_compiled[n], regexp[n].c_str(), REG_EXTENDED );
#endif
  }
}


Regexp::~Regexp()
{
  for( int n = 0; n < N; ++n ) regfree( &regex_compiled[n] );

  free( regex_compiled );
}


void Regexp::printResults( string str, int ret, regex_t regex )
{
    const int sz = 100;
    char errbuff[sz];

    regerror( ret, &regex, errbuff, sz );
    printf( "%s: %s\n", str.c_str(), errbuff );
}


regex_t Regexp::getCompiledRegexp( int ndx )
{
  if( ( ndx < 0 ) || ( ndx >= N ) ) throw( string { "Regexp::getCompiledRegexp" }  );

  return regex_compiled[ndx];
}