#include <stdlib.h>
#include <math.h>

#include "box.h"
#include "molevelset.h"

double inset_risk(box *p, double *y, double gamma, double A);
double complexity_penalty(box *p, int n, double delta);

int find_sibling(box *pb, box_split *ps, int dim) {
  /* Find the splits representing a sibling box.
   *
   * Args:
   *   pb: pointer to the box to find splits for.
   *   ps: pointer to splits, will be populated by this function.
   *   dim: which dimension to check.
   * Returns:
   *   BOX_SUCCESS if successfully calculated splits, BOX_ERROR otherwise.
   */
  if (copy_box_split2(ps, pb->split) != BOX_SUCCESS) {
    return BOX_ERROR;
  }
  
  /* Flip the last bit in the requested dimension. */
  ps->split[dim] ^= 1 << ps->nsplit[dim];
  
  return BOX_SUCCESS;
}

int find_parent(box *pb, box_split *ps, int dim) {
  /* Find the split representing the parent box. 
   *
   * Args:
   *   pb: pointer to box.
   *   ps: pointer to splits, will be populated by this function.
   *   dim: which dimension to check.
   * Returns:
   *   BOX_SUCCESS if successfully calculated splits, BOX_ERROR otherwise.
   */
  if (!pb || !ps || dim < 0 || dim > pb->split->d || ps->d != pb->split->d) {
    return BOX_ERROR;
  }
  
  
}

box_cost levelset_cost(box *p, double *y, int n, double gamma, double delta, 
		       double rho, double A) {
  /* Calculate the inset cost for a box.
   *
   * Args:
   *  p: pointer to box to calculate inset cost of,
   *  y: pointer to response values.  Indexed by p->i.
   *  n: integer, total number of points.
   *  gamma: double, level that we are calculating.
   *  delta: double, SOMETHING OR OTHER.
   *  rho: double, complexity penalty multiplier.
   *  A: double, maximum absolute value of the response variable.
   * Returns:
   *   populated box_cost struct.
   */
  double risk = inset_risk(p, y, gamma, A);
  double cost = rho * complexity_penalty(p, n, delta);
  box_cost ret;
  ret.inset = risk < 0;
  ret.cost = (risk < 0 ? 1 : -1) * risk + cost;
  return ret;
}

double inset_risk(box *p, double *y, double gamma, double A) {
  /* Calculate the inset risk for a box.
   *
   * Args:
   *  p: pointer to box to calculate inset cost of,
   *  y: pointer to response values.  Indexed by p->i.
   *  gamma: double, level that we are calculating.
   *  A: double, maximum absolute value of the response variable.
   * Returns:
   *   double, cost of the box if it is in the set.
   */
  if (!p->n) {
    return 0.0;
  }

  double risk = 0.0;
  for (int i = 0; i < p->n; i++) {
    risk += gamma - y[p->i[i]];
  }
  
  return risk / (2 * A);
}

double complexity_penalty(box *p, int n, double delta) {
  /* Compute the complexity penalty for a box.
   *
   * Args:
   *   p: pointer to bo to calculate penalty for.
   *   n: integer, total number of points.
   *   delta: double, complexity factor.
   * Returns:
   *   double, complexity penalty for this box.
   */
  int tree_level = 0; // The level of the tree := the sum of the number of splits.
  for (int j = 0; j < p->split->d; j++) {
    tree_level += p->split->nsplit[j];
  }
  double L = tree_level * (log2(p->split->d) + 2) + 1;

  double phat = ((double) p->n) / n;
  double pl = (L * log(2) + log(1 / delta)) / n;
  pl = 4 * (pl > phat ? pl : phat);
  
  return sqrt((8 * (log(2 / delta) + L * log(2)) * pl) / n);
}
