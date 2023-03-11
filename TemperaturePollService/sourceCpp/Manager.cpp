
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

  constructPollHandle();

  sensor = new WeatherSensor();
}


Manager::~Manager()
{
  delete sensor;
  destructPollHandle();

  curl_global_cleanup();
}


size_t Manager::writeData( void *buffer, size_t size, size_t nmemb, void *userp )
{
  // function "writeData" may be called more than once!
  // "buffer": pointer to new data to be saved
  // "size": always equal 1 (see curls docu)
  // "nmemb": size of data to be saved, maybe equal 0!
  // return value: size of processed data, 
  //    if unequal to nmemb -> CURLE_WRITE_ERROR will be generated by curl

  struct WriteMemoryStruct *mem = (struct WriteMemoryStruct *)userp;

  vector<char> buff( (char *)buffer, (char *)buffer + nmemb );
  mem->memory.insert( mem->memory.end(), buff.begin(), buff.end() );
  mem->size += nmemb;

  return nmemb;
}


void Manager::constructPollHandle()
{
    pollHandle = curl_easy_init();
    if( pollHandle == NULL ) throw( string { "Manager::constructPollHandle" } );

    // set parameters for reading from URL
    pollHeaders = {};

    curl_easy_setopt( pollHandle, CURLOPT_URL, URL );
    curl_easy_setopt( pollHandle, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( pollHandle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 );
    curl_easy_setopt( pollHandle, CURLOPT_WRITEFUNCTION, writeData );
    curl_easy_setopt( pollHandle, CURLOPT_WRITEDATA, (void *)&content );

    // pass our list of custom made headers
    curl_easy_setopt( pollHandle, CURLOPT_HTTPHEADER, pollHeaders );
}


void Manager::destructPollHandle()
{
    curl_slist_free_all( pollHeaders );
    curl_easy_cleanup( pollHandle );
}


void Manager::constructSendfileHandle()
{
}


void Manager::destructSendfileHandle()
{
}


string Manager::getRawDataString()
{
    content.memory = {};              // no data initially
    content.memory.reserve( 3000 );   // approximated max. size
    content.size = 0;

    CURLcode cres = curl_easy_perform( pollHandle );
    if( cres != CURLE_OK ) throw( string { "Manager::getRawDataString" } );

    return string( content.memory.begin(), content.memory.end() );
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
  sendHandle = new SendFileHandle( localFileName, remoteFileName );

  CURLcode cres = curl_easy_perform( sendHandle->getHandle() );
  if( cres != CURLE_OK ) throw( string { "Manager::transferDataFile" } );

  delete sendHandle;
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
    // get actual timestamp and in case of calendar day change provide actual 
    // local filename and filename for file transfer to NAS
    string timeStamp = manageTime();

    // read Zigbee gateways REST-API to get the rawdata string containing data
    // of all sensors, parse this rawdata string
    sensor->parseSensorData( timeStamp, getRawDataString() );

    // write parsed sensor data into file
    sensor->writeDataToFile( localFileName );

    // FTP-transfer local file to NAS
    transferDataFile();

    // sleep
    this_thread::sleep_for( sleepTime ); // wait TIMER_REPEAT_TIME
  }
}