
#include "Parser.h"


Parser::Parser()
{
    regexp = new Regexp();
}


Parser::~Parser()
{
    delete regexp;
}


const char *Parser::findMatchingCurlyBrace( const char *p )
{
    char c = *p;                    // read first char
    if( c == 0 ) return NULL;       // is it end of string: return error
    if( c != '{' ) return NULL;     // is it not starting bracket

    int counter = 0;                // count of open bracket pairs

    while( true )
    {
      c = *(++p);                   // read next char

      if( c == 0 ) return NULL;     // is it end of string: return error
      if( c == '}' )                // is it a matching bracket 
      {
        if( counter == 0 ) break;   // it is a matching bracketbut and no open pairs: success
        --counter;                  // it is a matching bracketbut and an open pairs: count it
      }

      if( c == '{' ) ++counter;     // it is a new pair of open brackets: count it
    }

    return p;   // pointer to corresponding bracket or NULL in case of error
}


string Parser::getSensorName( string config )
{
    regex_t regcomp1 = regexp->getCompiledRegexp( 1 );  // sensor name
    regex_t regcomp2 = regexp->getCompiledRegexp( 12 ); // specification of allowed sensornames

    int ret = regexec( &regcomp1, config.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret != 0 ) return string();         // no name available

    int startNdx = pmatch[0].rm_so; // index to first matched char
    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    string s = config.substr( endNdx, config.length() - endNdx );

    ret = regexec( &regcomp2, s.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret != 0 ) return string();         // no name available

    startNdx = pmatch[0].rm_so; // index to first matched char
    endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    return s.substr( startNdx, endNdx - 1 );
}


string Parser::getSensorConfig( string internal )
{
    regex_t regcomp = regexp->getCompiledRegexp( 2 ); // config
    return getSensorInternalData( internal, regcomp );
}


string Parser::getSensorState( string internal )
{
    regex_t regcomp = regexp->getCompiledRegexp( 3 ); // state
    return getSensorInternalData( internal, regcomp );
}


string Parser::getSensorInternalData( string internal, regex_t regcomp )
{
    int ret = regexec( &regcomp, internal.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret != 0 ) return string();

    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    string s = internal.substr( endNdx, internal.length() - endNdx );
    const char *p = findMatchingCurlyBrace( s.c_str() );
    if( p == NULL ) throw( string { "Parser::getSensorInternalData" } );

    string match = s.substr( 0, p - s.c_str() );

    return string( match.begin() + 1, match.end() );
}


bool Parser::isInterestingSensor( string config )
{
    regex_t regcomp = regexp->getCompiledRegexp( 4 ); // regex for battery

    int ret = regexec( &regcomp, config.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );

    if( ret == 0 ) return 1;  // sensor with battery
    else           return 0;  // sensor without battery
}


vector<string> Parser::getSensorsRawDataStrings( string rawData )
{
  // Reads for every Zigbee sensor its raw data string without leading "{" and terminating "}"
  // and stores it in element "allData" of "structure SensorRawData".
  // Create for every found sensor a new vector element.

    regex_t regcomp = regexp->getCompiledRegexp( 0 ); // regex for sensors "start sequence"

    string toBeMatchedNext = rawData;
    vector<string> sensorRawDataVector = {};

    while( true )   // loop until no further sonsor found
    {
        int ret = regexec( &regcomp, toBeMatchedNext.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
        if( ret != 0 ) break;   // no further sonsor found: leave loop

        // sensor found: index to first unmatched char after end of match, that is "{"
        int endNdx = pmatch[0].rm_eo;

        string s = toBeMatchedNext.substr( endNdx, toBeMatchedNext.length() - endNdx );
        const char *p = findMatchingCurlyBrace( s.c_str() );  // find end if sensors raw data string
        if( p == NULL ) throw( string { "Parser::getSensorsRawDataStrings" } );

        int len = p - s.c_str();
        string match = s.substr( 0, len + 1 );  // sensors raw data string including leading "{" and terminating "}"

        // remove leading "{" and terminating "}" from raw data string
        sensorRawDataVector.push_back( string( match.begin() + 1, match.end() - 1 ) );

        // calculate next match pointer, that is the "start sequence" of next sensor, if any
        toBeMatchedNext = string( p + 2, p + ( s.length() - len ) );
    }

    return sensorRawDataVector;
}


string Parser::getBatteryChargeString( string config )
{
    auto regcomp = regexp->getCompiledRegexp( 4 );    // regex for battery
    return getBatteryCharge( config, regcomp ); 
}


string Parser::getHumidityString( string state )
{
    auto regcomp = regexp->getCompiledRegexp( 5 );    // regex for humidity
    return getMeasuredValue( state, regcomp ); 
}


string Parser::getPressureString( string state )
{
    auto regcomp = regexp->getCompiledRegexp( 6 );    // regex for humidity
    return getMeasuredValue( state, regcomp ); 
}


string Parser::getTemperatureString( string state )
{
    auto regcomp = regexp->getCompiledRegexp( 7 );    // regex for temperature
    return getMeasuredValue( state, regcomp ); 
}


vector<string> Parser::getDateTimeString( string state )
{
    vector<string> ret( 2 );  // create 2-tupel

    auto regcomp = regexp->getCompiledRegexp( 8 );  // regex for lastupdated
    string lastupdated = getLastUpdated( state, regcomp );

    regcomp = regexp->getCompiledRegexp( 9 );       // regex for date
    string d = getDateTime( lastupdated, regcomp );
    if( d != string() ) ret[0] = d;

    regcomp = regexp->getCompiledRegexp( 10 );      // regex for time
    string t = getDateTime( lastupdated, regcomp );
    if( t != string() ) ret[1] = t;

    return ret;
}


string Parser::getBatteryCharge( string config, regex_t regcomp )
{
  int ret = regexec( &regcomp, config.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
  if( ret == 0 )  // battery should be always there (sensor is interesting) but anyway
  {
    int startNdx = pmatch[0].rm_so; // index to first matched char
    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    string s = config.substr( endNdx, config.length() - endNdx );

    auto regcomp1 = regexp->getCompiledRegexp( 11 );    // at least one signed integer
    ret = regexec( &regcomp1, s.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret == 0 )
    {
      startNdx = pmatch[0].rm_so; // index to first matched char
      endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

      return s.substr( startNdx, endNdx );
    }
  }

  return string();
}


string Parser::getMeasuredValue( string state, regex_t regcomp )
{
  int ret = regexec( &regcomp, state.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
  if( ret == 0 )
  {
    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match
    string s = state.substr( endNdx, state.length() - endNdx );

    auto regcomp1 = regexp->getCompiledRegexp( 11 );    // at least one signed integer
    int ret = regexec( &regcomp1, s.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret == 0 )
    {
      endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match
      return s.substr( 0, endNdx );
    }
  }

  return string();
}


string Parser::getLastUpdated( string state, regex_t regcomp )
{
  int ret = regexec( &regcomp, state.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
  if( ret == 0 )
  {
    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match
    return state.substr( endNdx, state.length() - endNdx );
  }

  return string();
}


string Parser::getDateTime( string dt, regex_t regcomp )
{
  int ret = regexec( &regcomp, dt.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
  if( ret == 0 )
  {
    int startNdx = pmatch[0].rm_so; // index to first matched char
    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    string s = dt.substr( startNdx, endNdx - startNdx );

    return s;
  }

  return string();
}