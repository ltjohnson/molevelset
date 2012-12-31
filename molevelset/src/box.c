#include <stdlib.h>

#include "box.h"

box *new_box(int d, unsigned int *splits, int *nsplits, int size_guess) {
  /* Create and initialize a new box. 
   *
   * Args:
   *   d: dimension of box.
   *   splits: pointer to splits for this box.
   *   nsplits: pointer to number of splits in each dimension for this box.
   *   size_guess: callers guess at number of points to expect.
   *     Numbers smaller than 1 are replaced with 1.
   * Returns:
   *   pointer to initialized box struct.
   */
  int j;
  box *p;

  /* Don't take a size_guess < 1. */
  size_guess = 1 > size_guess ? 1 : size_guess;
  
  p = (box *)malloc(sizeof(box));
  p->d = d;
  p->n = 0;
  p->isize = size_guess;
  p->i = (int *)malloc(sizeof(int) * size_guess);
  p->nsplits = (int *)malloc(sizeof(int) * d);
  p->splits = (unsigned int*)malloc(sizeof(unsigned int) * d);
  
  for (j = 0; j < d; j++) {
    p->nsplits[j] = nsplits[j];
    p->splits[j] = splits[j];
  }

  return p;
}

void free_box(box *p) {
  /* Free memory associated with a box.
   *
   * Args:
   *   p: pointer to box to free.
   */
  free(p->splits);
  free(p->nsplits);
  free(p->i);
  free(p);
}

void add_point(box *p, int i) {
  /* Add a point to a box. 
   *
   * Args:
   *   p: pointer to box.
   *   i: index of point to add.
   */
  if (p->n >= p->isize) {
    /* Increase size of array. */
    int j, new_size, *new_array;
    new_size = p->isize > 0 ? 2 * p->isize : 1;
    new_array = (int *)malloc(sizeof(int) * new_size);
    for (j = 0; j < p->n; j++) 
      new_array[j] = p->i[j];
    if (p->i)
      free(p->i);
    p->i = new_array;
  }

  p->i[p->n++] = i;
}

unsigned int point_to_split(double *px, int d, int k_max) {
  int i, go_left;
  double next_split = 0.5; 
  double divisor = 0.25;
  unsigned int split = 0;
  
  for (i = 0; i < k_max; i++) {
    /* go_left = <Is this point left of the next split?> */
    go_left = *px < next_split;
    /* Assign left or right to the current split. */
    split |= (go_left ? LEFT_SPLIT : RIGHT_SPLIT) << i;
    next_split = next_split + (go_left ? -1 : 1) / divisor;
    divisor = divisor / 2;
  }
}

void point_to_box(double *px, int d, int k_max, unsigned int *pbox) {
  int i;

  for(i = 0; i < d; i++) {
    pbox[i] = point_to_split(px + i, d, k_max);
  }
}

box_collection *new_collection(int d) {
  /* Create and intialize an empty collection of boxes.
   *
   * Args:
   *   d: number of dimenstions.
   * Returns:
   *   pointer to new collection.
   */
  box_collection *p = (box_collection *)malloc(sizeof(box_collection));
  p->boxes = NULL;
  p->n = 0;
  p->d = d;
  
  return p;
}

box_collection *points_to_boxes(double *px, int n, int d, int k_max) {
  /* Put a collection of points into boxes.
   *
   * Args:
   *   px: pointer to points to box, column centric array.
   *   n: number of points.
   *   d: dimension.
   *   k_max: max number of splits to use.
   * Returns:
   *   pointer to newly alloced box_collection. 
   */
  int i, j;
  unsigned int splits[d];
  int nsplits[d];
  double point[d];
  box *cur_box;
  box_collection *p_collection;

  /* Initialized once, nsplits is constant in this function. */
  for (j = 0; j < d; j++)
    nsplits[j] = k_max;

  p_collection = new_collection(d);
  
  for(i = 0; i < n; i++) {
    for(j = 0; j < d; j++)
      point[j] = px[i + j * n];

    point_to_box(point, d, k_max, splits);

    cur_box = find_box(p_collection, splits, nsplits);
    if (!cur_box) {
      cur_box = new_box(d, splits, nsplits, 1 + (n - i) / 4);
      add_box(p_collection, cur_box);
    }

    add_point(cur_box, i);
  }
  
  return p_collection;
}
