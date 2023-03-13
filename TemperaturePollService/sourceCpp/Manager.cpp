
#include <thread>
#include "Manager.h"


Manager::Manager()
{
  sleepTime = TIMER_REPEAT_TIME;  // initialize sleep timespan

  setTime( &actTime );            // initialize both time stamps
  oldTime = actTime;

  generateFileNames();            // initialize filenames

  CURLcode ret = curl_global_init( CURL_GLOBAL_ALL );
  if( ret != CURLE_OK ) throw( string { "Manager::Manager" } );

  zigbeeBridge = new ZigbeeBridge();
  weatherSensor = new ZigbeeWeatherSensor();
}


Manager::~Manager()
{
  delete weatherSensor;
  delete zigbeeBridge;

  curl_global_cleanup();
}


void Manager::setTime( struct tm *buffer )
{
  time_t t = time( NULL );
  localtime_r( &t, buffer );
}


string Manager::time2string( struct tm usedTM )
{
#define NALLOC 24   // maximal needed buffer size should be 20: "%02d.%02d.%04d %02d:%02d:%02d"

  char * cbuff = (char *)malloc( NALLOC );

  int ret = snprintf( cbuff, NALLOC, "%02d.%02d.%04d %02d:%02d:%02d",
    usedTM.tm_mday, usedTM.tm_mon + 1, usedTM.tm_year + 1900,
    usedTM.tm_hour, usedTM.tm_min, usedTM.tm_sec );

  string time( cbuff, 0, ret );

  free( cbuff );

  return time;
}


string Manager::manageTime()  // return actual timestamp and provide actual filenames
{
  oldTime = actTime;
  setTime( &actTime );

  // change of calender day -> create new filenames
  if( oldTime.tm_mday != actTime.tm_mday ) generateFileNames();

  return time2string( actTime );
}


void Manager::generateFileNames()
{
    // generate new local filename
    localFileName = string( FILENAME_PREFIX );
    string datetime = time2string( actTime );
    size_t ndx = datetime.find_first_of( ' ' );
    localFileName += string( datetime.begin(), datetime.begin() + ndx );
    localFileName += string( FILENAME_POSTFIX );

    // generate new remote filename
    remoteFileName = string( DISKSTATION );
    remoteFileName += localFileName;
}


void Manager::transferDataFile()
{
  NAS transfer( localFileName, remoteFileName );
  transfer.perform();  // perform curl actions
}


void Manager::executionLoop()
{
#ifdef DEBUG
  int count = 0;
  while( count <= 3 )
  {
    ++count;
#else
  while( true )
  {
#endif
    // Network: read Zigbee gateways REST-API to get the rawdata string containing all sensors
    string rawDataString = zigbeeBridge->getRawDataString();

    // get actual timestamp and in case of calendar day change provide actual 
    // local filename and filename for file transfer to NAS
    string timeStamp = manageTime();

    // parse rawdata string
    weatherSensor->parseSensorData( timeStamp, rawDataString );

    // write parsed weatherSensor data into file
    weatherSensor->writeDataToFile( localFileName );

    // Network: FTP-transfer local result file to NAS
    transferDataFile();

    // sleep
    this_thread::sleep_for( sleepTime ); // wait TIMER_REPEAT_TIME
  }
}