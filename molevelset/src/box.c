#include <stdlib.h>

#include "box.h"

box *newbox(int dim) {
  box	*p   = (box *)malloc(sizeof(box));
  int i;

  p->arr_max = 16;
  p->nbox    = 0;
  p->cost    = 0;
  p->Y	     = (double *)malloc(sizeof(double) * p->arr_max);
  p->id	     = (int *)malloc(sizeof(int) * p->arr_max);
  p->left    = p->right = NULL;
  p->checked = (int *)malloc(sizeof(int) * dim);
  p->splits  = NULL;
  for(i = 0; i < dim; i++)
    p->checked[i] = 0;

  return(p);
}

void deletebox(box *p) {
  free(p->Y);
  free(p->id);
  free(p->left);
  free(p->right);
  free(p->checked);
  free(p);
}

// Add the observation Y, with id to this box.
void addtobox(box *p, double Y, int id) {
  int i;
  if (p->nbox == p->arr_max) {
    double *tmp_Y  = (double *)malloc(sizeof(double) * 2 * p->arr_max);
    int	   *tmp_id = (int *)malloc(sizeof(int) * 2 * p->arr_max);
    for(i = 0; i<p->arr_max; i++) {
      tmp_Y[i]  = p->Y[i];
      tmp_id[i] = p->id[i];
    }
    free(p->id);
    free(p->Y);
    p->Y  = tmp_Y;
    p->id = tmp_id;
    p->arr_max = 2 * p->arr_max;
  }
  p->Y[p->nbox] = Y;
  p->id[p->nbox] = id;
  p->nbox++;
}

// Make the double array pointed to by vsplits into a key.
// converts each 'row' of the integer doublt array into an
// integer, adds 1, and  multiplies the rows together.
int make_key(void *vsplits) {
  int **s  = (int **)vsplits;
  int key = 1;
  int i,j;
  for(i = 0; s[i]; i++) {
    int tmp = 0;
    for( j = 0; s[i][j]; j++) 
      tmp = (tmp << 1) + s[i][j] - 1;
    key = key * tmp;
  }
  return( key );
}

// return 0 if keys match, non-zero otherwise.
// keys are integer double arrays.  Entires are in {1,2}.  
// First level of the array is terminated with a NULL pointer.
// Second level of the arrray is terminated with 0.
int cmp_keys(void *key1, void *key2) {
  int **k1 = (int **)key1;
  int **k2 = (int **)key2;
  int i,j;
  
  for(i = 0; ;i++) {
    if (!k1[i] && !k2[i])
      // keys match to the end.
      break;
    if (!k1[i] || !k2[i])
      return( 1 );
    for(j = 0; k1[i][j] || k2[i][j]; j++) {
      if (k1[i][j] != k2[i][j])
	return( 1 );
    }
  }
  return( 0 );
}
