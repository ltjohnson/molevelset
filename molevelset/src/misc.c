#include <stdlib.h>

#include "misc.h"
#include "box.h"

// take a point, X, in and find the splits necessary to 
// specify it's box.  For each dimension, 1 represents 
// the left side of a split, 2 represents the right 
// side of a split.
void findbox(double *X, int dim, int kmax, int **splits) {
  double lower, upper, mid;
  int d, k;

  for(d = 0; d < dim; d++) {
    splits[d][kmax] = 0;
    lower = 0.0;
    upper = 1.0;
    for(k = 0; k < kmax; k++) {
      mid = (lower + upper) / 2;
      splits[d][k] = X[d] <= mid ? 1 : 2;
      if (X[d] <= mid) {
	splits[d][k] = 1;
	upper = mid;
      } else {
	splits[d][k] = 2;
	lower = mid;
      }
    }
  }
}

int **copysplits(int **splits, int d, int kmax) {
  int **dst = (int **)malloc(sizeof(int *) * (d + 1));
  int i, j;

  dst[d]  = NULL;
  for(i=0; i < d; i++) {
    dst[i] = (int *)malloc(sizeof(int) * (kmax + 1));
    dst[i][kmax] = 0;
    for(j=0; j < kmax; j++)
      dst[i][j] = splits[i][j];
  }
  return( dst );
}

// Divide the points, X, up into the boxes.  Returns a hash
// of all the boxes found.  Each box in this hash contains at
// least 1 data point.
hash *getboxes(double **X, double *Y, int n, int d, int kmax) {

  box *b;
  hash *h;
  hash_entry *he;
  int **splits;
  int i;
  int size = 1;
  
  splits = (int **)malloc(sizeof(int *) * (d + 1));
  splits[d+1] = NULL;
  for(i=0; i<d; i++)
    splits[i] = (int *)malloc(sizeof(int) * (kmax + 1));
  
  size = 1;
  for(i = 0; i<kmax*d; i++)
    size <<= 1;
  h = make_hash( size, &make_key, &cmp_keys );
  
  for(i = 0; i < n; i++) {
    // find box
    findbox(X[i], d, kmax, splits);
    he = hash_find(h, splits);
    if (!he) {
      b = newbox(d);
      b->splits = copysplits(splits, d, kmax);
      he = hash_add(h, b->splits, b);
    }
    
    // add to box
    addtobox((box *)he->data, Y[i], i);
  }
  
  for(i = 0; i < d; i++)
    free(splits[i]);
  free(splits);

  return( h );
}
