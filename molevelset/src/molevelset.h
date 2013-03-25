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

typedef struct {
  double total_cost;     /* Total cost of the estimated levelset. */
  levelset_args la;      /* Args used to estimate the levelset. */
  int num_inset;         /* Number of final boxes in the levelset. */
  box **inset_boxes;     /* NULL terminated array of boxes in the levelset. */
  int num_non_inset;     /* Number of final boxes not in the levelset. */
  box **non_inset_boxes; /* NULL terminated array of boxes not in the levelset. */
} levelset_estimate;

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
 *   A populated levelset_estimate struct.
 */
levelset_estimate compute_levelset(box_collection *pinitial, levelset_args);
#endif
