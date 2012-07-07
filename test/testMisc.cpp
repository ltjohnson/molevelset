/* File to test the functions in misc.h */
#include "misc.h"
#include "box.h"

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

  cout << "  Checking 3 points in [0, 1]^2...\n";
  double x[] = {0.3, 0.7, 0.1, 0.4, 0.2, 0.8};
  double y[] = {1.0, 1.1, 1.2};
  /* Should find boxes with keys '1122', '1212', and '2111' containing
     points 2, 0, and 1, respectively.  */
  map<string, MOBox *> *pBoxMap = FindBoxes(3, 2, 2, x, y);

  cout << "    Checking for 3 boxes...";
  int nBoxes = pBoxMap->size();
  if (nBoxes == 3) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found " << nBoxes << ".\n";
    success = 0;
  }
  map<string, MOBox *>::iterator it= pBoxMap->begin();
  cout << "    Checking first box has key 1122 and point 2...";
  vector<int> points = it->second->GetPoints();
  if (it->first == "1122" && points.size() == 1 && points.at(0) == 2) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found key " << it->first;
    if (points.size()) {
      cout << " with first point " << points.at(0) << ".\n";
    } else {
      cout << " with no points.\n";
    }
    success = 0;
  }
  
  it++;
  cout << "    Checking second box has key 1212 and point 0...";
  points = it->second->GetPoints();
  if (it->first == "1212" && points.size() == 1 && points.at(0) == 0) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found key " << it->first;
    if (points.size()) {
      cout << " with first point " << points.at(0) << ".\n";
    } else {
      cout << " with no points.\n";
    }
    success = 0;
  }
  
  it++;
  cout << "    Checking third box has key 2111 and point 1...";
  points = it->second->GetPoints();
  if (it->first == "2111" && points.size() == 1 && points.at(0) == 1) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found key " << it->first;
    if (points.size()) {
      cout << " with first point " << points.at(0) << ".\n";
    } else {
      cout << " with no points.\n";
    }
    success = 0;
  }

  /* cleanup */
  for(it = pBoxMap->begin(); it != pBoxMap->end(); it++)
    delete it->second;
  delete pBoxMap;

  return(success);
}

int TestFindBoxesSingleBox() {
  int success = 1;
  cout << "TestFindBoxesSingleBox\n";
  
  double x[2];
  x[0] = 0.55; x[1] = 0.55;
  double y[1]; y[0] = 0.3;
  map<string, MOBox *> *pBoxMap = FindBoxes(1, 2, 3, x, y);
  
  cout << "  Checking for 1 box...";
  if (pBoxMap->size() == 1) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found " << pBoxMap->size() << " boxes.\n";
    return(0);
  }
  
  map<string, MOBox *>::iterator it = pBoxMap->begin();
  cout << "  Checking that box has key 211211...";
  if (it->first == "211211") {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Has key '" << it->first << "'.\n";
    success = 0;
  }
  
  cout << "  Checking that box has point 0...";
  vector<int> points = it->second->GetPoints();
  if (points.size() == 1 && points.at(0) == 0) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Found " << points.size() << " points.";
    if (points.size()) 
      cout << "  First point is point " << points.at(0) << ".";
    cout << "\n";
  }
  
  /* cleanup */
  delete pBoxMap;

  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  success *= TestBoxLength();
  success *= TestGetUnivariateSplits();
  success *= TestGetBoxSplits();
  success *= TestFindBoxesSingleBox();
  success *= TestFindBoxes();
  if (!success) {
    cout << "FAILURE.  Some tests failed.\n";
  } else {
    cout << "All tests passed.\n";
  }
  return(!success);
}
