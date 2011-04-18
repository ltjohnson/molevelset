#ifndef MISC_H
#define MISC_H

#include "hash.h"


hash *getboxes(double *X, double *Y, int n, int d, int kmax);

int make_key(void *); // make integer key out of box description
int cmp_keys(void *, void*); // return 0 if keys are equal, non zero otherwise

#endif
