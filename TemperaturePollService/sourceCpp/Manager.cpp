
#include <thread>
#include "Manager.h"


// initialized storage for static data-members
duration<int,ratio<1,1>> Manager::sleep_time { TIMER_REPEAT_TIME };

struct WriteMemoryStruct Manager::content {};
struct curl_slist *Manager::poll_headers {};
CURLM *Manager::poll_handle {};
CURLM *Manager::sendfile_handle {};
struct tm Manager::oldTime {};
struct tm Manager::actTime {};
string Manager::localFileName {};
string Manager::remoteFileName {};
FILE *Manager::fileToSend {};
Parser *Manager::parser {};
Sensor *Manager::sensor {};


// private functions
size_t Manager::write_data( void *buffer, size_t size, size_t nmemb, void *userp )
{
  // function "write_data" may be called more than once
  // "buffer": pointer to new data to be saved
  // "size": always equal 1
  // "nmemb": size of data to be saved, maybe equal 0
  // return value: size of processed data, 
  //    if unequal to nmemb -> CURLE_WRITE_ERROR will be generated by curl

  struct WriteMemoryStruct *mem = (struct WriteMemoryStruct *)userp;

  vector<char> buff( (char *)buffer, (char *)buffer + nmemb );
  mem->memory.insert( mem->memory.end(), buff.begin(), buff.end() );
  mem->size += nmemb;

  return nmemb;
}


void Manager::construct_poll_handle()
{
    poll_handle = curl_easy_init();
    if( poll_handle == NULL ) throw( string { "Manager::construct_poll_handle" }  );

    // set parameters for reading from URL
    poll_headers = {};

    curl_easy_setopt( poll_handle, CURLOPT_URL, URL );
    curl_easy_setopt( poll_handle, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( poll_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 );
    curl_easy_setopt( poll_handle, CURLOPT_WRITEFUNCTION, write_data );
    curl_easy_setopt( poll_handle, CURLOPT_WRITEDATA, (void *)&content );

    // pass our list of custom made headers
    curl_easy_setopt( poll_handle, CURLOPT_HTTPHEADER, poll_headers );
}


void Manager::destruct_poll_handle()
{
    curl_slist_free_all( poll_headers );
    curl_easy_cleanup( poll_handle );
}


string Manager::getRawDataString()
{
    content.memory = {};              // no data initially
    content.memory.reserve( 3000 );   // approximated max. size
    content.size = 0;

    CURLcode cres = curl_easy_perform( poll_handle );
    if( cres != CURLE_OK ) throw( string { "Manager::getRawDataString" }  );

    return string( content.memory.begin(), content.memory.end() );
}


void Manager::setTime( struct tm *buffer )
{
  time_t t = time( NULL );
  localtime_r( &t, buffer );
}


string Manager::time2string( struct tm usedTM )
{
#define NALLOC 25   // maximal needed buffer size should be 20: "%02d.%02d.%04d %02d:%02d:%02d"

  char * cbuff = (char *)malloc( NALLOC );

  int ret = snprintf( cbuff, NALLOC, "%02d.%02d.%04d %02d:%02d:%02d",
    usedTM.tm_mday, usedTM.tm_mon + 1, usedTM.tm_year + 1900,
    usedTM.tm_hour, usedTM.tm_min, usedTM.tm_sec );

  string time( cbuff, 0, ret );

  free( cbuff );

  return time;
}


string Manager::manageTime()  // return actual timestamp and provide actual filename for file transfer to NAS
{
  oldTime = actTime;
  setTime( &actTime );

  // change of calender day -> create new filenames
  if( oldTime.tm_mday != actTime.tm_mday ) generateFileNames();

  return time2string( actTime );
}


void Manager::generateFileNames()
{
    // generate new filename, deconstruct old curl-handle and construct a new one
    localFileName = string( FILENAME_PREFIX );

    string datetime = time2string( actTime );
    size_t ndx = datetime.find_first_of( ' ' );
    localFileName += string( datetime.begin(), datetime.begin() + ndx );
    localFileName += string( FILENAME_POSTFIX );

    remoteFileName = string( DISKSTATION );
    remoteFileName += localFileName;
}


void Manager::construct_sendfile_handle()
{
    sendfile_handle = curl_easy_init();
    if( sendfile_handle == NULL ) throw( string { "Manager::construct_sendfile_handle curl_easy_init" }  );

    fileToSend = fopen( localFileName.c_str(), "r" );
    if( fileToSend == NULL ) throw( string { "Manager::construct_sendfile_handle fopen" }  );

    // set curls parameters for filetransfer to NAS
    curl_easy_setopt( sendfile_handle, CURLOPT_URL, remoteFileName.c_str() );
    curl_easy_setopt( sendfile_handle, CURLOPT_READDATA, (void *)fileToSend );
    curl_easy_setopt( sendfile_handle, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( sendfile_handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );
    curl_easy_setopt( sendfile_handle, CURLOPT_USERPWD, FTPCREDENTIALS );
}


void Manager::destruct_sendfile_handle()
{
    int ret = fclose( fileToSend );
    if( ret != 0 ) throw( string { "Manager::destruct_sendfile_handle" }  );

    curl_easy_cleanup( sendfile_handle );
}


void Manager::transferDataFile()
{
  construct_sendfile_handle();

  CURLcode cres = curl_easy_perform( sendfile_handle );
  if( cres != CURLE_OK ) throw( string { "Manager::transferDataFile" }  );

  destruct_sendfile_handle();
}


// public functions
Manager::Manager()
{
  setTime( &actTime );    // initialize both time stamps
  oldTime = actTime;

  generateFileNames();    // initialize filenames

  CURLcode ret = curl_global_init( CURL_GLOBAL_ALL );
  if( ret != CURLE_OK ) throw( string { "Manager::Manager" } );

  construct_poll_handle();
  sensor = new Sensor();
}


Manager::~Manager()
{
  delete sensor;
  destruct_poll_handle();

  curl_global_cleanup();
}


void Manager::executionloop()
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

    // get actual timestamp and provide actual filename for file transfer to NAS
    string timeStamp = manageTime();

    // read Zigbee gateways REST-API to get the rawdata string containing data
    // of all sensors
    sensor->rawDataString = getRawDataString();

    // parse this rawdata string
    sensor->parseSensorsData( timeStamp );

    // write parsed sensor data into file
    sensor->writeDataToFile( localFileName );

    // FTP-transfer this file to NAS
    transferDataFile();

    // sleep
    this_thread::sleep_for( sleep_time ); // wait TIMER_REPEAT_TIME seconds
  }
}