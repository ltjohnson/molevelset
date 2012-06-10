/* File to test the functions in misc.hh */
#include "misc.hh"
#include "box.hh"

#include <iostream>
#include <vector>

using namespace::std;

int TestBoxLength() {
  int success = 1;
  int res;
  vector<BoxSplit> splits;

  cout << "TestBoxLength\n";
  
  /* 0 length vector. */
  cout << "  Checking BoxLength([]) == 0...";
  res = BoxLength(splits);
  if (res == 0) {
    cout << " Success.\n";
  } else {
    success = 0;
    cout << " FAILURE.  Got " << res << "\n";
  }
  
  /* 3 splits, all used. */
  cout << "  Checking BoxLength([LEFT_SPLIT, LEFT_SPLIT, RIGHT_SPLIT]) == " 
       << "3...";
  splits.push_back(LEFT_SPLIT);
  splits.push_back(LEFT_SPLIT);
  splits.push_back(RIGHT_SPLIT);
  res = BoxLength(splits);
  if (res == 3) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  got" << res << "\n";
    success = 0;
  }

  /* 5 splits, one stop. */
  cout << "  Checking BoxLength(LEFT_SPLIT, RIGHT_SPLIT, RIGHT_SPLIT, " <<
    "STOP_SPLIT, LEFT_SPLIT]) == 3...";
  splits.clear();
  splits.push_back(LEFT_SPLIT);
  splits.push_back(RIGHT_SPLIT);
  splits.push_back(RIGHT_SPLIT);
  splits.push_back(STOP_SPLIT);
  splits.push_back(LEFT_SPLIT);
  res = BoxLength(splits);
  if (res == 3) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  got" << res << "\n";
    success = 0;
  }
  
  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  if (!TestBoxLength())
    success = 0;
  if (!success) {
    cout << "FAILURE.  Some tests failed.\n";
  } else {
    cout << "All tests passed.\n";
  }
  return(!success);
}
