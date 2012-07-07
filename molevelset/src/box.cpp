#include "box.h"
#include "misc.h"
#include <math.h>
#include <sstream>

MOBox::MOBox(int kmax, int dim, int n, double *px, double *py, 
             vector< vector<BoxSplit> > splits) {
  this->n = n;
  this->kmax = kmax;
  this->dim = dim;
  this->px = px;
  this->py = py;
  // TODO: Throw exception if splits doesn't have the right shape.
  this->splits = splits;
  this->A = fabs(py[0]);
  for(int i = 1; i < n; i++) 
    if (this->A < fabs(py[i]))
      this->A = fabs(py[i]);
  for(int i = 0; i < dim; i++)
    this->checked.push_back(0);
  this->terminalNode = 1;
}
  
int MOBox::GetChecked(int i) {
  if (i < checked.size() && splits.at(i).size() && 
      splits.at(i)[0] != STOP_SPLIT)
    return(checked.at(i));
  else
    return(1);
}

void MOBox::SetChecked(int i) {
  checked.at(i) = 1;
}

void MOBox::AddPoint(int i) {
  pointsIndex.push_back(i);
}

void MOBox::AddPoint(vector<int> points) {
  for(int i = 0; i < points.size(); i++)
    this->AddPoint(points[i]);
}

vector<int> MOBox::GetPoints() {
  return(pointsIndex);
}

MORisk MOBox::Risk(double gamma) {
  MORisk ret;
  if (!terminalNode) {
    ret.inset = -1;
    ret.risk = children.at(0)->Risk(gamma).risk + 
      children.at(1)->Risk(gamma).risk;
    return(ret);
  }
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
  if (!terminalNode) {
    return(children.at(0)->Phi(delta) + children.at(1)->Phi(delta));
  }
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
  MOCost cost;
  if (!terminalNode) {
    cost.inset = -1;
    cost.cost = children.at(0)->Cost(gamma, delta, rho).cost +
      children.at(1)->Cost(gamma, delta, rho).cost;
    return(cost);
  }
  MORisk risk = Risk(gamma);
  double phi = Phi(delta);
  cost.cost = risk.risk + rho * phi;
  cost.inset = risk.inset;
  return(cost);
}

void MOBox::SetChildren(MOBox *p1, MOBox *p2) {
  children.clear();
  children.push_back(p1);
  children.push_back(p2);
  terminalNode = 0;
}

vector<MOBox *> MOBox::GetChildren() {
  return(children);
}

string MOBox::GetBoxKey() {
  return GetBoxKey(splits);
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

string MOBox::GetSiblingKey(int i) {
  if (!splits.size() || splits.size() < i)
    return "";
  vector<vector<BoxSplit> > sib = splits;
  int j = 0;
  while(j < sib.at(i).size() && sib.at(i).at(j) != STOP_SPLIT)
    j++;
  j--;
  if (j < 0) 
    return("");
  sib.at(i).at(j) = 
    (splits.at(i).at(j) == LEFT_SPLIT ? RIGHT_SPLIT : LEFT_SPLIT);
  return(GetBoxKey(sib));
}

string MOBox::GetParentKey(int i) {
  if (!splits.size() || splits.size() < i)
    return("");
  vector<vector<BoxSplit> > parent = splits;
  int j = 0;
  while(j < parent.at(i).size() && parent.at(i).at(j) != STOP_SPLIT)
    j++;
  j--;
  if (j < 0) 
    return("");
  parent.at(i).at(j) = STOP_SPLIT;
  return(GetBoxKey(parent));
}
