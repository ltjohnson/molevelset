#ifndef MOLEVELSET_H
#define MOLEVELSET_H

#include "box.h"

typedef struct {
  int inset;        /* 1 for inset, 0 for not. */
  double risk;      /* Risk associated with this box. */
  double cost;      /* cost for this box. */
  double risk_cost; /* risk + cost for this box. */
} box_cost;

/* levelset_cost calculates the box_cost for the given box.  This box_cost
 * is based on a complexity penalty (a function of delta, the number of
 * points and the splits representing this box), and the risk of the box (a
 * function of the gamma, A, and the y values for the points in the
 * box. 
 */
box_cost levelset_cost(box *p, double *y, int n, double gamma, double delta, 
		       double rho, double A);

/* Computes the levelset for a box collection.  
 *
 * pc: box collection .
 * y: double *, array of response values, has length n.
 * n: integer, number of points.
 * d: integer, number of dimensions.
 * kmax: integer, maximum number of splits.
 * gamma: double, threshold for the levelset.
 * delta: double, SOMETHING SOMETHING SOMETHING.
 * rho: double, SOMETHING SOMETHING SOMETHING.
 */
box_collection *compute_levelset(box_collection *pc, double *y, int n, 
				 int d, int kmax,  double gamma, double delta, 
				 double rho);
#endif
