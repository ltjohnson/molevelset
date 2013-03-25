#ifndef MOLEVELSET_H
#define MOLEVELSET_H

#include "box.h"

typedef struct {
  int d;        /* Dimension of X points. */
  int kmax;     /* Max number of splits in a single dimension. */
  int n;        /* Number of points. */
  double **x;   /* X points, location of the points. */
  double *y;    /* Response value of points. */
  double A;     /* Maximum absolute value of points in y. */
  double gamma; /* Threshold for the levelset. */
  double delta; /* Probability bound for the levelset calculation. */
  double rho;   /* Tree complexity penalty for levelset calculation. */
} levelset_args;

/* Compute the max value in a vector. */
double max_vector_fabs(double *y, int n);

/* levelset_cost calculates the box_cost for the given box.  This box_cost
 * is based on a complexity penalty (a function of delta, the number of
 * points and the splits representing this box), and the risk of the box (a
 * function of the gamma, A, and the y values for the points in the
 * box. 
 */
box_risk levelset_cost(box *, levelset_args *);

/* Computes the levelset for a box collection.  
 *
 * Args:
 *   pinitial: box collection.
 *   levelset_args: parametrs for the levelset.
 * Returns: 
 *   Pointer to null terminated array of boxes encoding the minimax
 *   optimal levelset.
 */
box **compute_levelset(box_collection *pinitial, levelset_args);
#endif
