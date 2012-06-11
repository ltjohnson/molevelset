/* File to test the functions in misc.hh */
#include "misc.hh"
#include "box.hh"

#include <iostream>
#include <vector>

using namespace::std;

int TestGetBoxKey() {
  int success = 1;
  
  cout << "TestGetBoxKey\n";
  vector<vector<BoxSplit> > split;

  cout << "  Checking GetBoxKey([ [LEFT_SPLIT, RIGHT_SPLIT, LEFT_SPLIT], " 
       << "[RIGHT_SPLIT, LEFT_SPLIT, STOP_SPLIT]]) == \"121210\"...";
  for(int i = 0; i < 2; i++) {
    vector<BoxSplit> tmp;
    split.push_back(tmp);
  }
  split.at(0).push_back(LEFT_SPLIT); 
  split.at(0).push_back(RIGHT_SPLIT); 
  split.at(0).push_back(LEFT_SPLIT); 
  split.at(1).push_back(RIGHT_SPLIT);
  split.at(1).push_back(LEFT_SPLIT);
  split.at(1).push_back(STOP_SPLIT);
  string expected = "121210";
  string key = MOBox::GetBoxKey(split);
  if (key == expected) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE. Got \"" << key << "\".\n";
    success = 1;
  }
  return(success);
}

int TestGetPoints() {
  int success = 1;
  cout << "TestGetPoints\n";
  
  cout << "  Checking that pBox->GetPoints().push_back(x) != pBox->GetPoints()...";
  vector<vector<BoxSplit> > splits;
  vector<BoxSplit> tmp;
  splits.push_back(tmp);
  splits.at(0).push_back(LEFT_SPLIT);
  double x[] = {0.1, 0.2, 0.3};
  double y[] = {0.0, 0.1, -1.2};
  MOBox *pBox = new MOBox(1, 1, 3, x, y, splits);
  pBox->AddPoint(0);
  vector<int> points = pBox->GetPoints();
  points.push_back(2);
  vector<int> actual = pBox->GetPoints();
  vector<int> expected;
  expected.push_back(0);
  if (expected == actual) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.\n";
    success = 0;
  }

  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  success *= TestGetBoxKey();
  success *= TestGetPoints();
  if (!success) {
    cout << "FAILURE.  Some tests failed.\n";
  } else {
    cout << "All tests passed.\n";
  }
  return(!success);
}
