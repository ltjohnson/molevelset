#include "box.hh"

int BoxLength(vector<BoxSplit> box) {
  int L;
  for(int i = 0; i < box.size() && box[i] != STOP_SPLIT; i++) 
    L++;
  return(L);
}

MOBox::MOBox(int kmax, int dim, int n, double **px, double *py, 
             vector< vector<BoxSplit> > splits) {
  n = n;
  kmax = kmax;
  dim = dim;
  px = px;
  py = py;
  // TODO: Throw exception if splits doesn't have the right shape.
  splits = splits;
}
  
void MOBox::AddPoint(int i) {
  pointsIndex.push_back(i);
}

MORisk MOBox::Risk(double gamma, double A) {
  MORisk ret = {.risk = 0, .inset = 1};
  for(int i = 0; i < pointsIndex.size(); i++) {
    ret.risk += gamma - py[pointsIndex[i]];
  }
  if (ret.risk < 0) {
    ret.risk *= -1;
    ret.inset = 0;
  }
  ret.risk = ret.risk / (2 * A);
  return(ret);
}

double MOBox::Phi(double delta) {
  double pnew;
  int L = 0;
  for(int i = 0; i < splits.size(); i++) {
    int tmpL = BoxLength(splits[i]);
    if (tmpL > L)
      L = tmpL;
  }
  pnew = L * log(2) - log(delta);
  if (nbox > pnew)
    pnew = nbox;
  pnew = 4 * pnew / n;
  return(sqrt((8 * log(2 / delta) + L * log(2)) * pnew / n));
}

MOCost MOBox::Cost(double gamma, double delta, double rho, double A) {
  MORisk risk = Risk(gamma, A);
  double phi = Phi(delta);
  MOCost cost = {.cost = risk.risk + rho * phi, .inset=risk.inset};
  return(cost);
}

string MOBox::GetBoxKey(vector<vector<BoxSplit>> box) {
  strinstream out;
  for(int i = 0; i < box.size(); i++) {
    for(int j = 0; j < box.size(); j++) {
      out << box[i][j];
    }
  }
  return(out.tostr())
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

void GetBoxSplits(double *x, int kmax, int d, vector<vector<BoxSplit> *p) {
  for(int i = 0; i < d; i++)
    GetUnivariateSplits(x[i], kmax, &p[i]);
}

map<string, *MOBox> FindBoxes(int n, ind d, int kmax, double **x, double *y) {
  map<string, *MOBox> *pBoxMap = new map<string, MOBox>;
  MOBox *pBox;
  vector<vector<BoxSplit>> box;
  for(int i = 0; i < n; i++) {
    GetBoxSplits(x[i], kmax, d, &box);
    string key = MOBox::GetBoxKey(box);
    map<string, *MOBox>::iterator it= pBoxMap->find(key);
    if (it == pBoxMap->end()) {
      /* This box isn't in the map, create and add it */
      pBox = new MOBox(kmax, d, n, x, y, &box);
      *pBoxMap[key] = pBox;
    } else {
      pBox = it->second;
    }
    pBox->AddPoint(i);
  }
  return(pBoxMap);
}
