#include "findtree.cc"

#include <vector>
#include <map>

vector<MOBox *> FindTree(double *px, double *py, int n, int dim, int kmax,
			 double gamma, double rho, double delta) {
  /* Initialize the hash for each level of the algorithm. */
  vector<map<string, MOBox *> *> D;
  D.push_back(FindBoxes(n, dim, kmax, px, py));

  /* Collapse one level at a time. */
  for(int i = 0; i < dim * kmax; i++) {
    D.push_back(CollapseLevel(D.at(i), dim, kmax, gamma, rho, delta));
  }

  /* Extract minimax optimal tree. */
  /* Cleanup. */
  for(int i = 0; i < D.size(); i++)
    delte D.at(i);

  return(COMPUTED VECTOR OF BOXES THAT IS THE TREE);
}

map<string, MOBox *> *CollapseLevel(map<string, MOBox *> *pOldLevel, int dim, 
				    int kmax, double gamma, double rho, 
				    double delta) {
  map<string, MOBox *> *pNewLevel = new map<string, MOBox *>;
}
