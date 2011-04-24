#ifndef BOX_H
#define BOX_H

typedef struct box {
  int nbox;
  int arr_max;
  int **splits;
  double *Y;
  int *id;
  double cost;
  int inset;
  struct box *left, *right;
  int *checked;
} box;

box *newbox(int dim);
void deletebox(box *);
void addtobox(box *, double Y, int id);

int make_key(void *); // make integer key out of box description
int cmp_keys(void *, void *); // return 0 if keys are equal, non zero otherwise

#endif
