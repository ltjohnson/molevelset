/* File to test the functions in misc.h */
#include "findtree.h"

#include <iostream>
#include <vector>

using namespace::std;

int TestFindTreeOnePoint() {
  int success = 1;
  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  success *= TestFindTreeOnePoint();
  cout << (success ? "All tests passed." : "FAILURE.  Some tests failed.") 
       << "\n";
  return(!success);
}
