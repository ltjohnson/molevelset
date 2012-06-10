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

int TestGetUnivariateSplits() {
  int success = 1;
  cout << "TestGetUnivariateSplits\n";
  vector<BoxSplit> splits, expected;

  cout << "  Checking GetUnivariateSplits(0.3, 3, *) => "
       << "[LEFT_SPLIT, RIGHT_SPLIT, LEFT_SPLIT]...";
  GetUnivariateSplits(0.3, 3, &splits);
  expected.clear();
  expected.push_back(LEFT_SPLIT);
  expected.push_back(RIGHT_SPLIT); 
  expected.push_back(LEFT_SPLIT);
  if (expected == splits) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  cout << "  Checking GetUnivariateSplits(0.3, 1, *) => [LEFT_SPLIT]...";
  expected.clear();
  expected.push_back(LEFT_SPLIT);
  GetUnivariateSplits(0.3, 1, &splits);
  if (expected == splits)  {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  cout << "  Checking GetUnivariateSplits(0.75, 1, *) => [RIGHT_SPLIT]...";
  expected.clear();
  expected.push_back(RIGHT_SPLIT);
  GetUnivariateSplits(0.75, 1, &splits);
  if (expected == splits)  {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  cout << "  Checking GetUnivariateSplits(0.5, 1, *) => [RIGHT_SPLIT]...";
  expected.clear();
  expected.push_back(RIGHT_SPLIT);
  GetUnivariateSplits(0.5, 1, &splits);
  if (expected == splits)  {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  return(success);
}

int TestGetBoxSplits() {
  int success = 1;
  cout << "TestGetBoxSplits\n";

  vector<vector<BoxSplit> > splits;
  vector<vector<BoxSplit> > expected;
  double x[6];
  
  cout << "  Checking GetBoxSplits([0.3], 1, 1, *) => [ [LEFT_SPLIT] ]...";
  x[0] = 0.3;
  GetBoxSplits(x, 1, 1, &splits);
  vector<BoxSplit> tmp;
  expected.push_back(tmp);
  expected.at(0).push_back(LEFT_SPLIT);
  if (expected == splits)  {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }
  
  cout << "  Checking GetBoxSplits([0.3, 0.3, 0.7], 1, 1, *) => "
       << "[ [LEFT_SPLIT], [LEFT_SPLIT], [RIGHT_SPLIT] ]...";
  x[0] = 0.3;
  x[1] = 0.3;
  x[2] = 0.7;
  GetBoxSplits(x, 1, 3, &splits);
  expected.clear();
  for(int i = 0; i < 3; i++) {
    vector<BoxSplit> tmp;
    expected.push_back(tmp);
  }
  expected.at(0).push_back(LEFT_SPLIT);
  expected.at(1).push_back(LEFT_SPLIT);
  expected.at(2).push_back(RIGHT_SPLIT);
  if (expected == splits)  {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  return(success);
}

int TestFindBoxes() {
  int success = 1;
  cout << "TestFindBoxes\n";

  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  success *= TestBoxLength();
  success *= TestGetUnivariateSplits();
  success *= TestGetBoxSplits();
  success *= TestFindBoxes();
  if (!success) {
    cout << "FAILURE.  Some tests failed.\n";
  } else {
    cout << "All tests passed.\n";
  }
  return(!success);
}
