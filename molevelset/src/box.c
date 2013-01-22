#include <stdlib.h>

#include "box.h"

/* Some private functions. */
unsigned int point_to_split(double *px, int d, int k_max);
void point_to_box(double *px, int d, int k_max, unsigned int *pbox);

box *new_box(box_split *split, int size_guess) {
  /* Create and initialize a new box. 
   *
   * Args:
   *   split: split to create.
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
  p->split = copy_box_split(split);
  p->n = 0;
  p->isize = size_guess;
  p->i = (int *)malloc(sizeof(int) * size_guess);

  return p;
}

void free_box(box *p) {
  /* Free memory associated with a box.
   *
   * Args:
   *   p: pointer to box to free.
   */
  free_box_split(p->split);
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

int compare_splits(box_split *ps1, box_split *ps2) {
  /* Compare two sets of splits, return 0 if unequal, 1 if equal.
   *
   * Args:
   *   ps1: pointer to box_split 1.
   *   ps2: pointer to box_split 2.
   * Returns:
   *   1 if splits are equal, 0 otherwise.
   */
  int i, j;
  unsigned int split_mask;

  if (!ps1 || !ps2) {
    return !ps1 & !ps2;
  }
  
  if (ps1->d != ps2->d) {
    return 0;
  }
  
  for (i = 0; i < ps1->d; i++) {
    if (ps1->nsplit[i] != ps2->nsplit[i]) {
      return 0;
    }
    
    split_mask = 0;
    for (j = 0; j < ps1->nsplit[i]; j++) {
      split_mask |= 1 << j;
    }
    if ((ps1->split[i] ^ ps2->split[i]) & split_mask) {
      return 0;
    }
  }
  
  return 1;
}

int split_to_interval(box_split *split, int dim, double *x1, double *x2) {
  /* Extract the real interval associated with a split.
   *
   * Args:
   *   split: pointer to the split.
   *   dim: which dimension to extract.
   *   x1: pointer to the left edge of the interval.
   *   x2: pointer to the right edge of the interval.
   * Returns:
   *   BOX_SUCCESS if split is succesfully converted, BOX_ERROR otherwise.
   */
  *x1 = 0;
  *x2 = 1;
  
  if (!split || dim < 0 || dim > split->d) {
    return BOX_ERROR;
  }
  
  for (int i = 0; i < split->nsplit[dim]; i++) {
    double next_split = (*x1 + *x2) / 2;
    if ((split->split[dim] & (1 << i)) == LEFT_SPLIT) {
      *x2 = next_split;
    } else {
      *x1 = next_split;
    }
  }
  
  return BOX_SUCCESS;
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
  double point[d];
  box *cur_box;
  box_collection *p_collection;
  box_split *p_split = new_box_split(d);

  /* Initialized once, nsplits is constant in this function. */
  for (j = 0; j < d; j++) {
    p_split->nsplit[j] = k_max;
  }

  p_collection = new_collection(d);
  
  for(i = 0; i < n; i++) {
    for(j = 0; j < d; j++) {
      point[j] = px[i + j * n];
    }

    point_to_box(point, d, k_max, p_split->split);

    cur_box = find_box(p_collection, p_split);
    if (!cur_box) {
      cur_box = new_box(p_split, 1 + (n - i) / 4);
      add_box(p_collection, cur_box);
    }

    add_point(cur_box, i);
  }
  
  free_box_split(p_split);

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
   *   BOX_SUCCESS if box is succesfully added, BOX_ERROR otherwise.
   */
  box_node *p_node;

  /* Don't add if one of the two pointers are NULL, or if this box is
   * already in the collection. */
  if (!pc || !pb || find_box(pc, pb->split)) {
    return BOX_ERROR;
  }
  
  p_node = (box_node *)malloc(sizeof(box_node));
  p_node->p = pb;
  p_node->next = pc->boxes;
  pc->boxes = p_node;
  pc->n++;
  
  return BOX_SUCCESS;
}

box *find_box(box_collection *pc, box_split *split) {
  /* Find a box in a collection that matches the given splits.
   *
   * Args:
   *   pc: pointer to box collection.
   *   split: pointer to box_split to find.
   * Returns:
   *   Returns pointer to box that matches the given splits if one is found, 
   *   NULL otherwise.
   */
  box_node *p_node = pc->boxes;

  while(p_node) {
    if (compare_splits(p_node->p->split, split)) { 
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

box_split *copy_box_split(box_split *split) {
  /* Make a copy of a box_split.
   *
   * Args:
   *   split: pointer to split to copy.
   * Returns:
   *   pointer to a copy of split.  NULL if split cannot be copied.
   */
  
  if (!split) {
    return NULL;
  }

  box_split *p = new_box_split(split->d);
  
  for (int i = 0; i < p->d; i++) {
    p->nsplit[i] = split->nsplit[i];
    p->split[i] = split->split[i];
  }

  return p;
}

int copy_box_split2(box_split *to, box_split *from) {
  /* Copy box split from one split to another.
   *
   * Args:
   *   to: pointer to destination split.
   *   from: pointer to source split.
   * Returns:
   *   BOX_SUCCESS if copy was succesful, BOX_ERROR otherwise.
   */
  if (!to || !from || to->d != from->d) {
    return BOX_ERROR;
  }
  
  for (int j = 0; j < to->d; j++) {
    to->split[j] = from->split[j];
    to->nsplit[j] = from->nsplit[j];
  }

  return BOX_SUCCESS;
}

void free_box_split(box_split *p) {
  /* Free the given box split. 
   *
   * Args:
   *   p: pointer to the box_split to be freed.
   */
  if (!p) {
    return;
  }
  
  free(p->split);
  free(p->nsplit);
  free(p);
}

box_split *new_box_split(int d) {
  /* Create a new box_split.
   *
   * Args: 
   *   d: number of dimensions to use.
   * Returns:
   *   pointer to the new box split.
   */
  box_split *p = (box_split *)malloc(sizeof(box_split));
  p->d = d;
  p->nsplit = (int *)malloc(d * sizeof(int));
  p->split = (unsigned int *)malloc(d * sizeof(unsigned int));
  
  return p;
}
