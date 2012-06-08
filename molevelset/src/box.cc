#include "box.hh"

MOBox::MOBox(int kmax, int dim, int n, double **px, double *py, 
             vector< vector<BoxSplit> > splits) {
  n = n;
  kmax = kmax;
  dim = dim;
  px = px;
  py = py;
  // Throw exception if splits doesn't have the right shape.
  splits;
}
  
void MOBox::AddPoint(int i) {
  this->pointsIndex.push_back(i);
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
  
}

MOCost MOBox::Cost(double gamma, double delta, double rho, double A) {
  MORisk risk = this->Risk(gamma, A);
  double phi = this->Phi(delta);
  MOCost cost = {.cost = risk.risk + rho * phi, .inset=risk.inset};
  return(cost);
}

string MOBox::GetBoxGet() {
  strinstream out;
  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < kmax; j++) {
      out << splits[i][j];
    }
  }
  return(out.tostr())
}

