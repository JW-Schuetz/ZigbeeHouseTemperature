
#include <thread>
#include "Sensors.h"


int main()
{
  Sensors *sensors = new Sensors();

  thread sThread { sensors->executionloop };  // create thread
  if( sThread.joinable() ) sThread.join();          // wait for threads ending

  // following code should never be reached
  delete sensors;
  return -1;
}