#ifndef FINDTREE_H
#define FINDTREE_H

#include "box.h"
#include "misc.h"

/* Find the minimax tree for the n points given by *px in [0, 1]^dim
   (n-by-d array stored in column-major format) *py.  kmax splits are
   used, with level gamma.  rho is the complexity penalty.  Tree is
   minimax optimal up to probability 1-delta. */
vector<MOBox *> FindTree(double *px, double *py, int n, int dim, int kmax, 
			 double gamma, double rho, double delta);
#endif
