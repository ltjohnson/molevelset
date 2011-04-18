#include "misc.h";

hash *getboxes(double *X, double *Y, int n, int d, int kmax) {

  hash *h;
  h = make_hash( 2**(kmax*d), &make_key, &cmp_keys );
  
  for(int i = 0; i < n; i++) {
    // find box
    // add to box
  }
  
}
