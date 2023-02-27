
#include "sensors.h"


size_t write_data( void *buffer, size_t size, size_t nmemb, void *userp )
{
  size_t realsize = size *nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc( mem->memory, mem->size + realsize + 1 );
  if( ptr == NULL )
  {   // Der Returnwert von 0 signalisiert CURL, dass ein Fehler auftrat.
      // CURL liefert dann CURLE_WRITE_ERROR.
    fprintf( stderr, "\nNot enough memory\n\n" );
    return 0;
  }
 
  mem->memory = ptr;
  memcpy( &(mem->memory[mem->size]), buffer, realsize );
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}