
#include <iostream>
#include <thread>
#include "Manager.h"


int main()
{
  try
  {
    Manager *manager = new Manager();

    thread sThread { manager->executionloop };  // create thread
    if( sThread.joinable() ) sThread.join();    // wait for threads ending

    // following code should never be reached
    delete manager;
  }
  catch( string e )
  {
    std::cerr << "Runtime exception at: " << e << '\n'<< '\n';
  }

  return -1;
}