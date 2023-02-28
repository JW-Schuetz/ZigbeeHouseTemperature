
#include <thread>
#include "Manager.h"


int main()
{
  Manager *manager = new Manager();

  thread sThread { manager->executionloop };  // create thread
  if( sThread.joinable() ) sThread.join();          // wait for threads ending

  // following code should never be reached
  delete manager;
  return -1;
}