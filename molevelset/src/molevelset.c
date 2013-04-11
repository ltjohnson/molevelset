#include <stdlib.h>
#include <math.h>

#include <R.h>
#include <Rinternals.h>

#include "box.h"
#include "molevelset.h"


void print_box(box *);
void print_collection(box_collection *);
box *combine_boxes(box *p1, box *p2, int dim, levelset_args *);
double inset_risk(box *p, levelset_args *);
double complexity_penalty(box *p, int n, double delta);
levelset_estimate initialize_levelset_estimate(box *, levelset_args);

double max_vector_fabs(double *y, int n) {
  /* Compute the maximum absolute value of a vector.
   *
   * Args:
   *   y: pointer to double array.
   *   n: integer, length of the array pointed to be y.
   * Returns:
   *   double, maximum value of all values in y.
   */
  if (n <= 0) {
    return -1;
  }
  double m = fabs(y[0]);
  for (int i = 1; i < n; i++) {
    double tmp = fabs(y[i]);
    if (tmp > m)
      m = tmp;
  }
  
  return m;
}

int find_sibling(box *pb, box_split *ps, int dim) {
  /* Find the splits representing a sibling box.
   *
   * Args:
   *   pb: pointer to the box to find splits for.
   *   ps: pointer to splits, will be populated by this function.
   *   dim: which dimension to check.
   * Returns:
   *   BOX_SUCCESS if successfully calculated splits, BOX_ERROR otherwise.
   */
  if (copy_box_split2(ps, pb->split) != BOX_SUCCESS) {
    return BOX_ERROR;
  }
  
  /* The sibling box comes from flipping the last split in the given
   * dimension.  For example, suppose we are looking at the box, that in
   * projected into this dimension, the box occupies the interval [0,
   * 0.25).  This box would be reprsented by the splits (Left, Left) (in
   * order).  Flipping the last split results in the splits (Left, Right)
   * (also in order), which corresponds to the interval [0.25, 0.5).  Our
   * internal representation can do this by XOR ing the bit in the last
   * split position with 1.
   */
  if (ps->nsplit[dim]) {
    ps->split[dim] ^= 1 << ps->nsplit[dim];
  }
  
  return BOX_SUCCESS;
}

int find_parent(box *pb, box_split *ps, int dim) {
  /* Find the split representing the parent box. 
   *
   * Args:
   *   pb: pointer to box.
   *   ps: pointer to splits, will be populated by this function.
   *   dim: which dimension to check.
   * Returns:
   *   BOX_SUCCESS if successfully calculated splits, BOX_ERROR otherwise.
   */
  if (dim < 0 || dim >= pb->split->d) {
    return BOX_ERROR;
  }

  if (copy_box_split2(ps, pb->split) != BOX_SUCCESS) {
    return BOX_ERROR;
  }
  
  /* A parent box is the one found by removing the last split in the given
   * dimension.  Consider the box that occupies the interval [0.75, 0.875),
   * which corresponds to the splits (Right, Right, Left).  The parent box,
   * is defined by the splits (Right, Right), which is the interval [0.75,
   * 1].  We can delete by decreasing the split counter for this dimension.
   */ 
  if (ps->nsplit[dim]) {
    ps->nsplit[dim]--;
  }
  
  return BOX_SUCCESS;
}

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

box *combine_boxes(box *p1, box *p2, int dim, levelset_args *la) {
  /* Combine two boxes. 
   *
   * Args:
   *   p1: box pointer, first box being combined.
   *   p2: box pointer, second box being combined.
   *   dim: integer, dimension being combined to create a parent box.
   *   la: levelset_args pointer, holds parameter values for the levelset 
   *     algorithm.
   * Returns:
   *   pointer to the box containing the combined boxes.
   */
  if (!p2 && !p1) 
    return NULL;
  if (!p1) {
    p1 = p2;
    p2 = NULL;
  } 
  
  /* Make parent box. */
  int parent_size = p1->points.n + (p2 ? p2->points.n : 0);
  box_split *parent_split = copy_box_split(p1->split);
  remove_split(parent_split, dim);
  box *parent = new_box(parent_split, parent_size);
  
  /* Combine the two boxes. */
  for (int i = 0; i < p1->points.n; i++) 
    add_point(parent, p1->points.i[i]);
  if (p2) 
    for (int i = 0; i < p2->points.n; i++)
      add_point(parent, p2->points.i[i]);
  
  /* Calculate the cost as if the parent box were a terminal node. */
  box_risk parent_risk = levelset_cost(parent, la);
  double existing_risk_cost = 
    p1->risk.risk_cost + (p2 ? p2->risk.risk_cost : 0);
  if (parent_risk.risk_cost < existing_risk_cost) {
    parent->terminal_box = 1;
    parent->risk = parent_risk;
  } else {
    parent->terminal_box   = 0;
    parent->risk.risk_cost = existing_risk_cost;
    parent->children[0]    = p1;
    parent->children[1]    = p2;
  }
  
  /* Return new box. */
  return parent;
}

box_risk levelset_cost(box *p, levelset_args *la) {
  /* Calculate the inset cost for a box.
   *
   * Args:
   *  p: pointer to box to calculate inset cost of,
   *  la: pointer to levelset_args, contains parameter values for the leveset
   *    algorithm.
   * Returns:
   *   populated box_cost struct.
   */
  box_risk ret;
  ret.inset_risk = inset_risk(p, la);
  ret.cost = la->rho * complexity_penalty(p, la->n, la->delta);
  ret.inset = ret.inset_risk < 0 ? 1 : 0;
  ret.risk_cost = (ret.inset ? 1 : -1) * ret.inset_risk + ret.cost;
  ret.calculated = 1;
  return ret;
}

double inset_risk(box *p, levelset_args *la) {
  /* Calculate the inset risk for a box.
   *
   * Args:
   *  p: pointer to box to calculate inset cost of,
   *  levelset_args: pointer to levelset_args, contains parameters for the 
   *    aglorithm. 
   * Returns:
   *   double, cost of the box if it is in the set.
   */
  if (!p->points.n) {
    return 0.0;
  }

  double risk = 0.0;
  for (int i = 0; i < p->points.n; i++)
    risk += la->gamma - la->y[p->points.i[i]];
  
  return risk / (2 * la->A);
}

double complexity_penalty(box *p, int n, double delta) {
  /* Compute the complexity penalty for a box.
   *
   * Args:
   *   p: pointer to bo to calculate penalty for.
   *   n: integer, total number of points.
   *   delta: double, complexity factor.
   * Returns:
   *   double, complexity penalty for this box.
   */
  /* The level of the tree is defined as the sum of the number of splits in
   * each dimension. 
   */
  int tree_level = 0; 
  for (int j = 0; j < p->split->d; j++) {
    tree_level += p->split->nsplit[j];
  }
  double L = tree_level * (log2(p->split->d) + 2) + 1;

  double phat = ((double) p->points.n) / n;
  double pl = (L * log(2) + log(1 / delta)) / n;
  pl = 4 * (pl > phat ? pl : phat);
  
  return sqrt((8 * (log(2 / delta) + L * log(2)) * pl) / n);
}

box_collection *minimax_step(box_collection *src, levelset_args *la) {
  /* Perform one step of the algorithm.
   *
   * Args:
   *   src: pointer to box collection.
   *   la: pointer to levelset_args, parameters for the algorithm.
   * Returns:
   *   pointer to new box_collection, contains boxes found by collapsing this
   *   level of the box_collection.
   */
  box_collection *dst = new_box_collection(src->d);
  
  if (!src->n) 
    return dst;

  box **arr = list_boxes(src);
  if (!arr) 
    /* No boxes in collection, return empty collection. */
    return dst;
  
  for (int i = 0; i < src->n; i++) {
    box * cur = arr[i];
    for (int j = 0; j < cur->split->d; j++) {
      /* Skip if this split has already been checked by a sibling box. */
      if (cur->checked[j]) 
	continue;
      
      /* Skip if there aren't any splits in this dimension. */
      if (!cur->split->nsplit[j]) {
	cur->checked[j] = 1;
	continue;
      }
      
      /* Find the sibling box and combine this box with it. */
      box *sib = find_box_sibling(src, cur->split, j);
      box *parent = combine_boxes(cur, sib, j, la);
      
      /* If the parent box is already in the tree, keep the one with the
	 lowest risk. */
      box *existing_parent = find_box(dst, parent->split);
      if (existing_parent) {
	if (existing_parent->risk.risk_cost < parent->risk.risk_cost) {
	  free_box_but_not_children(parent);
	} else {
	  remove_box(dst, existing_parent->split);
	  add_box(dst, parent);
	}
      } else {
	add_box(dst, parent);
      }
    }
  }

  free(arr);
  return dst;
}


levelset_estimate compute_levelset(box_collection *pinitial, levelset_args la) {
  /* Compute the levelset for the boxes contained in *pinitial.
   *
   * Args:
   *   pinitial: pointer to box collection.  The collection and the 
   *     contained boxes will be freed.
   *   la: levelset_args, the parameter values for the levelset algorithm.
   * Returns:
   *   levelset_estimate struct.
   */
  la.A = max_vector_fabs(la.y, la.n);

  /* Maximum depth of the tree, up to kmax splits in each of the d
     dimensions, plus 1 for no splits. */
  int max_depth = la.d * la.kmax + 1;

  box_collection *pc[max_depth];
  pc[0] = pinitial;
  /* Calculate all of the costs for the initial boxes. */
  box_node *node = pc[0]->boxes;
  while (node) {
    node->p->risk = levelset_cost(node->p, &la);
    node = node->next;
  }
  
  /* Collapse levels, one at a time, bottom (most splits) to top (no
     splits). */
  for (int i = 1; i < max_depth; i++)
    pc[i] = minimax_step(pc[i - 1], &la);
  
  levelset_estimate le  = initialize_levelset_estimate(get_first_box(pc[max_depth - 1]),
						       la);
  
  /* Cleanup.  Because we've copied the terminal nodes from the final tree
   * into an array, cleanup is very simple.  Each node still in memory is
   * contained in exactly one collection in pc.  So we just go through all
   * of the collections and free boxes at each level.  */
  for (int i = 0; i < max_depth; i++) 
    free_collection(pc[i]);
  
  return le;
}

levelset_estimate initialize_levelset_estimate(box *p, levelset_args la) {
  /* Convert a box and levelset_args into a levelset estimate.
   *
   * Args:
   *   box: pointer to box to convert.  Memory is not touched.
   *   la: levelset args used to compute the levelset estimate.
   * Returns:
   *   levelset_estimate struct.
   */
  levelset_estimate le;

  le.total_cost = p->risk.risk_cost;
  le.la = la;

  box **pfinal = get_terminal_boxes(p);

  le.num_inset = 0;
  le.num_non_inset = 0;
  /* This takes two passes to copy out the boxes.  Could be better. */
  int i = 0;
  while (pfinal[i]) {
    if (pfinal[i]->risk.inset)
      le.num_inset++;
    else
      le.num_non_inset++;
    i++;
  }

  le.inset_boxes = (box **)malloc(sizeof(box *) * (le.num_inset + 1));
  le.non_inset_boxes = (box **)malloc(sizeof(box *) * (le.num_non_inset + 1));
  int i_inset = 0;
  int i_non_inset = 0;
  i = 0;
  while (pfinal[i]) {
    if (pfinal[i]->risk.inset) 
      le.inset_boxes[i_inset++] = pfinal[i];
    else
      le.non_inset_boxes[i_non_inset++] = pfinal[i];
    i++;
  }
  
  le.inset_boxes[i_inset] = NULL;
  le.non_inset_boxes[i_non_inset] = NULL;
    
  free(pfinal);
  
  return(le);
}

void print_collection(box_collection *pc) {
  printf("box collection %d boxes.\n", pc->n);
  box_node *node = pc->boxes;
  int i = 0;
  while (node) {
    printf("  [%d]: ", i++);
    print_box(node->p);
    node = node->next;
  }
}

void print_box(box *p) {
  printf("box: ");
  print_split(p->split);
  printf(" points: {");
  for (int i = 0; i < p->points.n; i++) {
    printf("%d, ", p->points.i[i]);
  }
  printf("} inset: %d terminal_box: %d", p->risk.inset, p->terminal_box);
  if (!p->terminal_box) {
    printf(" child[0] = %p child[1] = %p", (void *)p->children[0], (void *)p->children[1]);
  }
  printf("\n");
}