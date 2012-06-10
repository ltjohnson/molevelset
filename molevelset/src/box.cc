#include "box.hh"
#include "misc.hh"
#include <math.h>
#include <sstream>

MOBox::MOBox(int kmax, int dim, int n, double *px, double *py, 
             vector< vector<BoxSplit> > splits) {
  n = n;
  kmax = kmax;
  dim = dim;
  px = px;
  py = py;
  // TODO: Throw exception if splits doesn't have the right shape.
  splits = splits;
  A = fabs(py[0]);
  for(int i = 1; i < n; i++) 
    if (A < fabs(py[i]))
      A = fabs(py[i]);
}
  
void MOBox::AddPoint(int i) {
  pointsIndex.push_back(i);
}

MORisk MOBox::Risk(double gamma) {
  MORisk ret;
  ret.risk = 0; ret.inset = 1;
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

MOCost MOBox::Cost(double gamma, double delta, double rho) {
  MORisk risk = Risk(gamma);
  double phi = Phi(delta);
  MOCost cost;
  cost.cost = risk.risk + rho * phi;
  cost.inset = risk.inset;
  return(cost);
}

string MOBox::GetBoxKey(vector<vector<BoxSplit> > box) {
  stringstream out;
  for(int i = 0; i < box.size(); i++) {
    for(int j = 0; j < box[i].size(); j++) {
      out << box[i][j];
    }
  }
  return(out.str());
}

