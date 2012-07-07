/* File to test the functions in misc.h */
#include "misc.h"
#include "box.h"

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

int TestBoxGetSiblingKey() {
  int success = 1;
  
  cout << "TestBoxGetSiblingKey\n";
  double x[] = {0.1, 0.2, 0.4, 0.5};
  double y[] = {0.25, 1};
  vector<vector<BoxSplit> > splits;
  for(int i = 0; i < 2; i++) {
    vector<BoxSplit> tmp;
    splits.push_back(tmp);
  }
  splits.at(0).push_back(LEFT_SPLIT);
  splits.at(0).push_back(STOP_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  MOBox *pBox = new MOBox(2, 2, 2, x, y, splits);
  
  string sibKey = pBox->GetSiblingKey(0);
  string key = pBox->GetBoxKey();
  
  cout << "  Checking key == \"1022\" && sibKey == \"2022\"...";
  if (key == "1022" && sibKey == "2022") {
    cout << " Success.\n";
  } else {
    success = 0;
    cout << " FAILURE.  Got key == \"" << key << "\" and sibKey == \"" 
	 << sibKey << "\".\n";
  }
  delete pBox;
  return(success);
}

int TestGetParentKey() {
  int success = 1;
  cout << "TestGetParentKey\n";

  double x[] = {0.1, 0.2, 0.4, 0.5};
  double y[] = {0.25, 1};
  vector<vector<BoxSplit> > splits;
  for(int i = 0; i < 2; i++) {
    vector<BoxSplit> tmp;
    splits.push_back(tmp);
  }
  splits.at(0).push_back(LEFT_SPLIT);
  splits.at(0).push_back(STOP_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  MOBox *pBox = new MOBox(2, 2, 2, x, y, splits);

  string parent = pBox->GetParentKey(0);
  string key = pBox->GetBoxKey();
  
  cout << "  Checking key == \"1022\" && parent == \"0022\"...";
  if (key == "1022" && parent == "0022") {
    cout << " Success.\n";
  } else {
    success = 0;
    cout << " FAILURE.  Got key == \"" << key << "\" and parent == \"" 
	 << parent << "\".\n";
  }
  delete pBox;

  return(success);
}

int TestBoxCreateParentBox() {
  int success = 1;

  cout << "TestBoxCreateParentBox\n";
  double x[] = {0.1, 0.2, 0.4, 0.5};
  double y[] = {0.25, 1};
  vector<vector<BoxSplit> > splits;
  for(int i = 0; i < 2; i++) {
    vector<BoxSplit> tmp;
    splits.push_back(tmp);
  }
  splits.at(0).push_back(LEFT_SPLIT);
  splits.at(0).push_back(STOP_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  splits.at(1).push_back(RIGHT_SPLIT);
  MOBox *pBox = new MOBox(2, 2, 2, x, y, splits);
  pBox->AddPoint(1);
  MOBox *pParent = pBox->CreateParentBox(1);

  string key = pBox->GetBoxKey();
  string parentKey = pParent->GetBoxKey();

  cout << "  Checking key == \"1022\" && parentKey == \"1020\"...";
  if (key == "1022" && parentKey == "1020") {
    cout << " Success.\n";
  } else {
    success = 0;
    cout << " FAILURE.  Got key == \"" << key << "\" and parentKey == \""
	 << parentKey << "\".\n";
  }
  cout << "  Checking parent contains point 1...";
  vector<int> expected;
  expected.push_back(1);
  if (expected == pParent->GetPoints()) {
    cout << " Success.\n";
  } else {
    success = 0;
    cout << " FAILURE.  Parent has points ";
    vector<int> points = pParent->GetPoints();
    for(int i = 0; i < points.size(); i++)
      cout << points[i] << ", ";
    cout << ".\n";
  }
  delete pBox;
  delete pParent;
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

int TestAddPoints() {
  int success = 1;
  cout << "TestAddPoints\n";
  
  cout << "  Checking that pBox->AddPoints([1, 2]) stores points 1 and 2...";
  vector<vector<BoxSplit> > splits;
  vector<BoxSplit> tmp;
  splits.push_back(tmp);
  splits.at(0).push_back(LEFT_SPLIT);
  double x[] = {0.1, 0.2, 0.3};
  double y[] = {0.0, 0.1, -1.2};
  MOBox *pBox = new MOBox(1, 1, 3, x, y, splits);
  vector<int> points;
  points.push_back(1); 
  points.push_back(2);
  pBox->AddPoint(points);
  vector<int> actual = pBox->GetPoints();
  if (points == actual) {
    cout << " Success.\n";
  } else {
    cout << " FAILURE.  Got [";
    for(int i = 0; i < actual.size(); i++) 
      cout << actual[i] << ", ";
    cout << "]\n";
    success = 0;
  }
  return(success);
}

int main(int argc, char**argv) {
  int success = 1;
  success *= TestGetBoxKey();
  success *= TestBoxGetSiblingKey();
  success *= TestGetParentKey();
  success *= TestBoxCreateParentBox();
  success *= TestGetPoints();
  success *= TestAddPoints();
  if (!success) {
    cout << "FAILURE.  Some tests failed.\n";
  } else {
    cout << "All tests passed.\n";
  }
  return(!success);
}
