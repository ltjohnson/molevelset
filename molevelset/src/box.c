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

int compare_splits(int d, unsigned int *s1, int *ns1, unsigned int *s2, int *ns2) {
  /* Compare two sets of splits, return 0 if unequal, 1 if equal.
   *
   * Args:
   *   d: number of dimensions.
   *   s1: pointer to splits of first box.
   *   ns1: pointer to number of splits of first box.
   *   s2: pointer to splits of second box.
   *   ns2: pointer to number of splits of second box.
   * Returns:
   *   1 if splits are equal, 0 otherwise.
   */
  int i, j;
  unsigned int split_mask;

  if (!s1 || !s2 || !ns1 || !ns2) {
    return !s1 & !s2 & !ns1 & !ns2;
  }
  
  for (i = 0; i < d; i++) {
    if (ns1[i] != ns2[i]) {
      return 0;
    }
    split_mask = 0;
    for (j = 0; j < ns1[i]; j++) {
      split_mask |= 1 << j;
    }
    if ((s1[i] ^ s2[i]) & split_mask) {
      return 0;
    }
  }
  
  return 1;
}

void split_to_interval(unsigned int split, int nsplit, double *x1, double *x2) {
  /* Convert a univariate split to the real interval defined by the split.
   *
   * Args:
   *   split: the split.
   *   nsplit: number of splits.
   *   x1: pointer to the left edge of the interval.
   *   x2: pointer to the right edge of the interval.
   */
  *x1 = 0;
  *x2 = 1;
  
  for (int i = 0; i < nsplit; i++) {
    double next_split = (*x1 + *x2) / 2;
    if ((split & (1 << i)) == LEFT_SPLIT) {
      *x2 = next_split;
    } else {
      *x1 = next_split;
    }
  }
}

unsigned int point_to_split(double *px, int d, int k_max) {
  double next_split = 0.5; 
  double divisor = 0.25;
  unsigned int split = 0;
  
  for (int i = 0; i < k_max; i++) {
    /* go_left = <Is this point left of the next split?> */
    int go_left = *px < next_split;
    split |= (go_left ? LEFT_SPLIT : RIGHT_SPLIT) << i;
    next_split = next_split + (go_left ? -1 : 1) * divisor;
    divisor = divisor / 2;
  }
  
  return split;
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
    for(j = 0; j < d; j++) {
      point[j] = px[i + j * n];
    }

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

/**************************************************************************
 * Functions for manipulating box collections.
 **************************************************************************/
int add_box(box_collection *pc, box *pb) {
  /* Add a box to this collection.
   *
   * Args:
   *   pc: pointer to collection.
   *   pb: pointer to box to add.
   * Returns:
   *   1 if box is succesfully added, 0 otherwise.
   */
  box_node *p_node;

  /* Don't add if one of the two pointers are NULL, or if this box is
   * already in the collection. */
  if (!pc || !pb || find_box(pc, pb->splits, pb->nsplits)) {
    return 0;
  }
  
  p_node = (box_node *)malloc(sizeof(box_node));
  p_node->p = pb;
  p_node->next = pc->boxes;
  pc->boxes = p_node;
  pc->n++;
  
  return 1;
}

box *find_box(box_collection *pc, unsigned int *splits, int *nsplits) {
  /* Find a box in a collection that matches the given splits.
   *
   * Args:
   *   pc: pointer to box collection.
   *   splits: pointer to splits for each dimension.
   *   nsplits: pointer to number of splits in each dimension.
   * Returns:
   *   Returns pointer to box that matches the given splits if one is found, 
   *   NULL otherwise.
   */
  box_node *p_node = pc->boxes;

  while(p_node) {
    if (compare_splits(pc->d, p_node->p->splits, p_node->p->nsplits, splits, 
		       nsplits)) {
      return p_node->p;
    }
    p_node = p_node->next;
  }
  
  return NULL;
}

void free_collection(box_collection *p) {
  /* Free a box collection, all boxes are free-ed.
   *
   * Args:
   *   p: pointer to box collection to free.
   */
  box_node *p_node, *tmp;

  if (!p) {
    return;
  }
  
  p_node = p->boxes;
  while (p_node) {
    tmp = p_node->next;
    free_box(p_node->p);
    free(p_node);
    p_node = tmp;
  }
  
  free(p);
}
