#include "misc.hh"

int BoxLength(vector<BoxSplit> box) {
  int L;
  for(int i = 0; i < box.size() && box[i] != STOP_SPLIT; i++) 
    L++;
  return(L);
}

void GetUnivariateSplits(double x, int kmax, vector<BoxSplit> *p) {
  p->clear();
  double curSize = 0.5;
  double cur = 0.5;
  for(int i = 0; i < kmax; i++) {
    p->push_back(x < cur ? LEFT_SPLIT : RIGHT_SPLIT);
    curSize = curSize / 2;
    cur = cur + (x < cur ? -1 : 1) * curSize;
  } 
}

void GetBoxSplits(double *x, int kmax, int d, vector<vector<BoxSplit> > *p) {
  for(int i = 0; i < d; i++)
    GetUnivariateSplits(x[i], kmax, &p->at(i));
}

map<string, MOBox *> *FindBoxes(int n, int d, int kmax, double *x, double *y) {
  double tmpX[d];
  map<string, MOBox *> *pBoxMap = new map<string, MOBox *>;
  MOBox *pBox;
  vector<vector<BoxSplit> > box;
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < d; j++) 
      tmpX[j] = x[j*n + i];
    GetBoxSplits(tmpX, kmax, d, &box);
    string key = MOBox::GetBoxKey(box);
    map<string, MOBox *>::iterator it= pBoxMap->find(key);
    if (it == pBoxMap->end()) {
      /* This box isn't in the map, create and add it */
      pBox = new MOBox(kmax, d, n, x, y, box);
      pBoxMap->insert(pair<string, MOBox *>(key, pBox));
    } else {
      pBox = it->second;
    }
    pBox->AddPoint(i);
  }
  return(pBoxMap);
}
