#ifndef MOLEVELSET_H
#define MOLEVELSET_H

typedef struct {
  int inset;   /* 1 for inset, 0 for not. */
  double cost; /* cost for this box. */
} box_cost;

box_cost levelset_cost(box *p, double *y, int n, double gamma, double delta, double rho, double A);

#endif
