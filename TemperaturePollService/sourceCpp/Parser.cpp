
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

    regmatch_t pmatch[1];
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
    regmatch_t pmatch[1];
    int ret = regexec( &regcomp, internal.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
    if( ret != 0 ) return string();

    int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

    string s = internal.substr( endNdx, internal.length() - endNdx );
    const char *p = findMatchingCurlyBrace( s.c_str() );
    if( p == NULL ) throw( string { "Parser::getSensorInternalData" }  );

    string match = s.substr( 0, p - s.c_str() );

    return string( match.begin() + 1, match.end() );
}


bool Parser::isInterestingSensor( string config )
{
    regex_t regcomp = regexp->getCompiledRegexp( 4 ); // regex for battery

    regmatch_t pmatch[1];
    int ret = regexec( &regcomp, config.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );

    if( ret == 0 ) return 1;  // sensor with battery
    else           return 0;  // sensor without battery
}


list<string> Parser::sensorsNames( vector<struct SensorRawData>sdata )
{
    list<string> sensorsNames = {};   // declare an empty list

    for( auto & iter: sdata )
        if( iter.interesting != 0 )   // process only interesting sensors (with a battery)
          sensorsNames.push_back( iter.name );

    sensorsNames.unique();  // make sensorsNames unique

    return sensorsNames;
}


vector<struct SensorRawData> Parser::getSensorsRawDataStrings( string rawData )
{
    regmatch_t pmatch[1];
    regex_t regcomp = regexp->getCompiledRegexp( 0 ); // regex for sensor rawdata string

    string toBeMatchedNext = rawData;
    vector<struct SensorRawData> sensorRawDataVector = {};

    while( true )   // loop until no further sonsor found
    {
        int ret = regexec( &regcomp, toBeMatchedNext.c_str(), ARRAY_SIZE( pmatch ), pmatch, 0 );
        if( ret != 0 ) break;   // no further sonsor found: leave loop

        int endNdx = pmatch[0].rm_eo;   // index to first unmatched char after end of match

        string s = toBeMatchedNext.substr( endNdx, toBeMatchedNext.length() - endNdx );
        const char *p = findMatchingCurlyBrace( s.c_str() );
        if( p == NULL ) throw( string { "Parser::getSensorsRawDataStrings" }  );

        int len = p - s.c_str();
 
        string match = s.substr( 0, len + 1 );

        // initialize structure, remove leading "{" and terminating "}"
        struct SensorRawData sensorRawData = {};
        sensorRawData.allData = string( match.begin() + 1, match.end() - 1 );

        sensorRawDataVector.push_back( sensorRawData );

        // calculate next match pointer
        toBeMatchedNext = string( p + 1, p + ( s.length() - len ) );
    }

    return sensorRawDataVector;
}


vector<struct PhysicalSensorsData> Parser::getMeasurementData( list<string> sensorNames,
        vector<struct SensorRawData> rawData, string timeStamp )
{
    int physSensorsCount = sensorNames.size();

    vector<struct PhysicalSensorsData> sensordata(physSensorsCount);  // declare vector of length physSensorsCount

    // for every physical sensor parse rawDataString
    auto iterName = sensorNames.begin();
    for( auto & iterData: sensordata )
    {
        // sensorname, timestamp
        iterData.sensorname = *(iterName++);
        iterData.owntime = timeStamp;

        // accumulate data of all virtual sensors with equal name
        for( auto & iterRawData: rawData )
        {
            if( iterRawData.interesting != 0 )   // process only interesting sensors (with a battery)
            {
                if( iterData.sensorname == iterRawData.name )
                {
                    // batterycharge: write only once for every physical sensor
                    if( iterData.batterycharge == string() )
                    {
                        auto regcomp = regexp->getCompiledRegexp( 4 );    // battery
                        string b = getBatteryCharge( iterRawData.config, regcomp ); 
                        if( b != string() ) iterData.batterycharge = b;
                    }

                    // humidity: write only once for every physical sensor
                    if( iterData.humidity == string() )
                    {
                        auto regcomp = regexp->getCompiledRegexp( 5 );    // humidity
                        string r = getMeasuredValue( iterRawData.state, regcomp );
                        if( r != string() ) iterData.humidity = r;
                    }

                    // pressure: write only once for every physical sensor
                    if( iterData.pressure == string() )
                    {
                        auto regcomp = regexp->getCompiledRegexp( 6 );    // pressure
                        string p = getMeasuredValue( iterRawData.state, regcomp );
                        if( p != string() ) iterData.pressure = p;
                    }

                    // temperature: write only once for every physical sensor
                    if( iterData.temperature == string() )
                    {
                        auto regcomp = regexp->getCompiledRegexp( 7 );    // temperature
                        string t = getMeasuredValue( iterRawData.state, regcomp );
                        if( t != string() ) iterData.temperature = t;
                    }

                    // sensordate, sensortime: write only once for every physical sensor
                    if( iterData.sensordate == string() )
                    {
                        auto regcomp = regexp->getCompiledRegexp( 8 );    // lastupdated
                        string lastupdated = getLastUpdated( iterRawData.state, regcomp );
                        if( lastupdated == string() ) string();
                        
                        regcomp = regexp->getCompiledRegexp( 9 );    // date
                        string d = getDateTime( lastupdated, regcomp );
                        if( d != string() ) iterData.sensordate = d;

                        regcomp = regexp->getCompiledRegexp( 10 );    // time
                        string t = getDateTime( lastupdated, regcomp );
                        if( t != string() ) iterData.sensortime = t;
                    }
                }
            }
        }

        iterData.state = SENSOR_OK;   // set valid
  }

  return sensordata;
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