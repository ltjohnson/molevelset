#include <stdlib.h>
#include <string.h>

#include <R.h>
#include <Rinternals.h>

#include "box.hh"

using namespace::std;

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

  p->checked = (int *)malloc(sizeof(int) * split->d);
  memset(p->checked, 0, sizeof(int) * split->d);
  
  /* Boxes default to terminal because they don't have children. */
  p->terminal_box = 1;

  p->points.n = 0;
  p->points.isize = size_guess;
  p->points.i = (int *)malloc(sizeof(int) * size_guess);

  p->risk.calculated = 0;
  p->risk.inset = -1;
  
  return p;
}

box * copy_box(box *src) {
  /* Copy a box.
   *
   * Args:
   *   src: pointer to the box to copy.
   *
   * Returns:
   *   pointer to the new box.
   */
  box *dst = (box *)malloc(sizeof(box));
  dst->split = copy_box_split(src->split);

  dst->points.n = src->points.n;

  dst->points.isize = src->points.isize;
  dst->points.i = (int *)malloc(sizeof(int) * dst->points.isize);
  memcpy(dst->points.i, src->points.i, sizeof(int) * dst->points.isize);

  dst->checked = (int *)malloc(sizeof(int) * dst->split->d);
  memcpy(dst->checked, src->checked, sizeof(int) * dst->split->d);
  
  dst->risk = src->risk;

  return dst;
}

void free_box_but_not_children(box *p) {
  /* Free memory associated with a box.
   *
   * Args:
   *   p: pointer to box to free.
   */
  free_box_split(p->split);
  free(p->checked);
  free(p->points.i);
  free(p);
}

void add_point(box *p, int i) {
  /* Add a point to a box. 
   *
   * Args:
   *   p: pointer to box.
   *   i: index of point to add.
   */
  box_points *points = &p->points;
  if (points->n >= points->isize) {
    /* Increase size of array. */
    int j, new_size, *new_array;
    new_size = points->isize > 0 ? 2 * points->isize : 1;
    new_array = (int *)malloc(sizeof(int) * new_size);
    for (j = 0; j < points->n; j++) 
      new_array[j] = points->i[j];
    if (points->i)
      free(points->i);
    points->i = new_array;
  }

  points->i[points->n++] = i;
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

box_collection *new_box_collection(box_split_info *info) {
  /* Create and intialize an empty collection of boxes.
   *
   * Args:
   *   info: pointer to box split info.
   * Returns:
   *   pointer to new collection.
   */
  box_collection *p = (box_collection *)malloc(sizeof(box_collection));
  p->info = info;
  p->h = new map<BoxSplitKey, box *>;
  
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
  box_split_info *info = new_box_split_info(d, k_max);

  /* Initialized once, nsplits is constant in this function. */
  for (j = 0; j < d; j++) {
    p_split->nsplit[j] = k_max;
  }

  p_collection = new_box_collection(info);
  
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

  /* Error if one or more pointers are NULL. */
  if (!pc || !pb) {
    return BOX_ERROR;
  }
  
  /* Create key for this box. */
  BoxSplitKey bsk(pb->split, pc->info);
  if (pc->h->find(bsk) != pc->h->end()) {
    return BOX_ERROR;
  }

  pc->h->operator[](bsk) = pb;
  
  return BOX_SUCCESS;
}

int remove_box(box_collection *pc, box_split *split) {
  /* Remove and delete the box with the specified split from the colection.
   *
   * Args:
   *   pc: pointer to box collection.
   *   split: pointer to box_split to remove.
   * Returns:
   *   BOX_SUCCESS if box is succesfully removed or is not in the
   *   collection, BOX_ERROR otherwise.
   */
  if (!pc) {
    return BOX_ERROR;
  } 
  if (!split) {
    return BOX_SUCCESS;
  }
  
  BoxSplitKey bsk(split, pc->info);
  map<BoxSplitKey, box *>::iterator it = pc->h->find(bsk);
  
  if (it == pc->h->end()) {
    return BOX_SUCCESS;
  }
  
  free_box_but_not_children(it->second);
  pc->h->erase(it);
  
  return BOX_SUCCESS;
}

int box_collection_size(box_collection *pc) {
  /* Get the number of boxes in a collection.
   *
   * Args:
   *   pc: pointer to box collection.
   * Returns:
   *   integer, number of boxes in pc.
   */
  if (!pc) {
    return 0;
  }
  return pc->h->size();
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
  if (!pc || !split) {
    return NULL;
  }

  BoxSplitKey bsk(split, pc->info);
  map<BoxSplitKey, box *>::iterator it = pc->h->find(bsk);
  if (it == pc->h->end()) {
    return NULL;
  }
  
  return it->second;
}

box *find_box_sibling(box_collection *pc, box_split *ps, int dim) {
  /* Find the sibling of a box in a collection.
   *
   * Args:
   *   pc: pointer to box collection.
   *   ps: pointer to box_split to find.
   *   dim: integer, which dimension to change.
   * Returns:
   *   Returns pointer to box that matches the input split, with the last 
   *   split  in the given direction reversed.  NULL if there are no splits 
   *   in that direction or the box isn't found.
   */
  if (!pc || !ps || !ps->nsplit[dim]) {
    return NULL;
  }
  
  box_split *s = copy_box_split(ps);
  s->split[dim] ^= 1 << (s->nsplit[dim] - 1);
  box *sib = find_box(pc, s);
  free_box_split(s);

  return sib;
}

box **list_boxes(box_collection *src) {
  /* Get an array listing the boxes in a collection.
   *
   * Args:
   *  src: box_collection pointer to access.
   * Returns:
   *  pointer to the array containing the boxes, NULL terminated;
   */
  if (!src)
    return NULL;
  box **arr = (box **)malloc(sizeof(box *) * (src->h->size() + 1));

  int i = 0;
  for (map<BoxSplitKey, box *>::iterator it = src->h->begin();
       it != src->h->end();
       it++) {
    arr[i++] = it->second;
  }

  arr[i] = NULL;

  return arr;
}

box *get_first_box(box_collection *p) {
  /* Get the first box from a collection.
   *
   * Args:
   *   p: box_collection pointer, collection to access.
   * Returns:
   *   pointer to the box, or NULL.
   */
  if (!p) {
    return NULL;
  }
  map<BoxSplitKey, box *>::iterator it = p->h->begin();
  return it == p->h->end() ? NULL : it->second;
}

void free_collection(box_collection *p) {
  /* Free a box collection, all boxes are free-ed.
   *
   * Args:
   *   p: pointer to box collection to free.
   */
  if (!p) {
    return;
  }
  
  for (map<BoxSplitKey, box *>::iterator it = p->h->begin();
       it != p->h->end();
       it++) {
    free_box_but_not_children(it->second);
  }
  
  delete p->h;
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
  p->d = split->d;
  
  for (int i = 0; i < p->d; i++) {
    p->nsplit[i] = split->nsplit[i];
    p->split[i] = split->split[i];
  }

  return p;
}

int remove_split(box_split *split, int dim) {
  /* Remove the box split in the specified dimension. 
   * 
   * Args:
   *   split: pointer to box_split to remove.
   *   dim: integer, dimension to remove split from.
   * Returns:
   *   BOX_SUCCESS if split is successfull removed, BOX_ERROR otherwise.
   */
  if (!split)
    return BOX_ERROR;
  
  if (!split->nsplit[dim]) 
    return BOX_SUCCESS;
  
  unsigned int split_mask = 0;
  for (int i = 0; i < split->nsplit[dim] - 1; i++) 
    split_mask |= (1 << i);
  split->split[dim] &= split_mask;
  split->nsplit[dim]--;
  
  return BOX_SUCCESS;
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

  p->d      = d;
  p->nsplit = (int *)malloc(d * sizeof(int));
  p->split  = (unsigned int *)malloc(d * sizeof(unsigned int));
  
  return p;
}

int box_split_key_hash_type(int d, int kmax) {
  int total_max_splits = d * kmax;
  if (total_max_splits <= sizeof(unsigned int) * CHAR_BIT)
    return KEY_UNSIGNED_LONG_LONG;
  else
    return KEY_STRING;
}

typedef struct box_node {
  box_node *next;
  box *p;
} box_node;

box_node *_get_boxes(box *box, box_node *node) {
  /* Recursively iterate on the tree contained in box. 
   * 
   * Args:
   *   box: pointer to box.
   *   box_node: pointer to node linked list, may be null.
   * Returns:
   *   Pointer to box_node containing the linked list of terminal boxes.
   */
  if (!box)
    return node;

  if (box->terminal_box) {
    box_node *tmp  = (box_node *)malloc(sizeof(box_node));
    tmp->p         = box;
    tmp->next      = node;
    return tmp;
  }
  box_node *ret = _get_boxes(box->children[0], node);
  ret = _get_boxes(box->children[1], ret);
  
  return ret;
}

box **get_terminal_boxes(box *p) {
  /* Get an array containing copies of the terminal boxes in a collection.
   *
   * Args:
   *   p: pointer to box to get terminal nodes from. 
   * Returns:
   *   Array containing pointers to copies of the terminal boxes in a 
   *   collection.
   */
  box **ret = NULL;
  if (!p) {
    ret = (box **)malloc(sizeof(box *));
    ret[0] = NULL;
    return ret;
  }

  box_node *terminal_boxes = _get_boxes(p, NULL);
  
  int nboxes = 0;
  box_node *tmp = terminal_boxes;
  while (tmp) {
    nboxes++;
    tmp = tmp->next;
  }
  
  if (!nboxes) {
    ret = (box **)malloc(sizeof(box *));
    ret[0] = NULL;
    return ret;
  }
  
  ret = (box **)malloc(sizeof(box *) * (nboxes + 1));
  tmp = terminal_boxes;
  for (int i = 0; i < nboxes; i++) {
    ret[i] = copy_box(tmp->p);
    tmp = tmp->next;
  }
  ret[nboxes] = NULL;
  
  /* Free the temporary linked list. */
  while (terminal_boxes) {
    tmp = terminal_boxes->next;
    free(terminal_boxes);
    terminal_boxes = tmp;
  }

  if (!ret) {
    ret = (box **)malloc(sizeof(box *));
    ret[0] = NULL;
  }

  return ret;
}

box_split_info *new_box_split_info(int d, int kmax) {
  /*  */
  box_split_info *info = (box_split_info *)malloc(sizeof(box_split_info));
  info->d = d;
  info->kmax = kmax;
  info->key_hash_type = box_split_key_hash_type(d, kmax);
  return info;
}

BoxSplitKey::BoxSplitKey() {
}

BoxSplitKey::BoxSplitKey(box_split *ps, box_split_info *pi) {
  SetSplit(ps, pi);
}

void BoxSplitKey::SetSplit(box_split *ps, box_split_info *pi) {
  key_hash_type = pi->key_hash_type;
  switch(key_hash_type) {
  case KEY_UNSIGNED_LONG_LONG:
    SetSplitULL(ps, pi);
    break;
  case KEY_STRING:
    SetSplitString(ps, pi);
    break;
  }
}

void BoxSplitKey::SetSplitULL(box_split *ps, box_split_info *pi) {
  /* We can encode all of the splits inside of an unsigned long long. */
  lkey = 0;

  int shift = 0;
  /* First encode the number of splits. */
  for (int i = 0; i < pi->d; i++) {
    lkey |= ps->nsplit[i] << shift;
    shift += pi->kmax;
  }

  /* Now encode the splits in each direction. */
  for (int i = 0; i < pi->d; i++) {
    /* If we guaranteed a set state for the unused split bits, we 
     * wouldn't need this inner loop. */
    lkey |= 
      (ps->split[i] & ((1 << ps->nsplit[i]) - 1)) << shift;
    shift += pi->kmax;
  }
}

void BoxSplitKey::SetSplitString(box_split *ps, box_split_info *pi) {
  skey = "";
  
  for (int i = 0; i < pi->d; i++) {
    skey += (ps->split[i] & (1 << ps->nsplit[i] - 1)) + " ";
  }
}

bool BoxSplitKey::operator<(const BoxSplitKey &right) const {
  switch (key_hash_type) {
  case KEY_UNSIGNED_LONG_LONG:
    return lkey < right.lkey;
    break;
  case KEY_STRING:
    return skey < right.skey;
    break;
  default:
    return false;
  }
}

void BoxSplitKey::print_key() const {
  switch(key_hash_type) {
  case KEY_UNSIGNED_LONG_LONG:
    printf("%llu", lkey);
    break;
  case KEY_STRING:
    printf("%s", skey.c_str());
    break;
  default:
    return;
  }
}

/* Output functions, used for debugging. */
void print_split(box_split *s) {
  printf("{");
  for (int i = 0; i < s->d; i++) {
    printf("[%d:%d:", i, s->nsplit[i]);
    for (int j = 0; j < s->nsplit[i]; j++) {
      int tmp = (s->split[i] & (1 << j)) >> j;
      printf(" %d", tmp + 1);
    }
    printf("] ");
  }
  printf("}");
}

void print_collection(box_collection *pc) {
  printf("box collection %d boxes.\n", box_collection_size(pc));
  box **boxes = list_boxes(pc);
  int boxPos = 0;
  while(boxes[boxPos]) {
    printf("  [%d]: ", boxPos);
    print_box(boxes[boxPos]);
    boxPos++;
  }
  free(boxes);
}

void print_box(box *p) {
  if (p == NULL) {
    printf("box: NULL\n");
    return;
  }

  printf("box: ");
  print_split(p->split);
  printf(" inset: %d terminal_box: %d points: %d risk_cost: %f", p->risk.inset, 
	 p->terminal_box, p->points.n, p->risk.risk_cost);
  if (!p->terminal_box) {
    printf(" child[0] = %p child[1] = %p", 
	   (void *)p->children[0], (void *)p->children[1]);
  }
  printf("\n");
}
