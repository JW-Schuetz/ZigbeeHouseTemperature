
#include <iostream>
#include "Manager.h"


int main()
{
  try
  {
    Manager *manager = new Manager();
    manager->executionloop();

    // the following code should never be reached
    delete manager;
  }
  catch( string e )
  {
    std::cerr << "Runtime exception at: " << e << '\n'<< '\n';
  }

  return -1;
}