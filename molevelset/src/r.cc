#include <R.h>
#include <Rinternals.h>

#include "box.hh"
#include "molevelset.hh"


extern "C" {
  SEXP box_to_list(box *p);

  SEXP get_boxes(SEXP X, SEXP k_max) {
    SEXP ans, dim;
    box_collection *pc;
    
    PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
    if (LENGTH(dim) != 2) {
      error("X must be a 2 dimensional matrix.");
    }
  
    if (LENGTH(k_max) != 1 || TYPEOF(k_max) != INTSXP) {
      error("k_max must be an integer of length 1.");
    }
  
    pc = points_to_boxes(REAL(X), INTEGER(dim)[0], INTEGER(dim)[1], 
			 INTEGER(k_max)[0]);
  
    /* Copy boxes out to a list. */
    int boxCount = box_collection_size(pc);
    PROTECT(ans = allocVector(VECSXP, boxCount));
    box **boxes = list_boxes(pc);
    for (int i = 0; i < boxCount; i++) {
      SET_VECTOR_ELT(ans, i, box_to_list(boxes[i]));
    }
    free(boxes);
  
    free_collection(pc);

    UNPROTECT(2);

    return ans;
  }


  SEXP box_to_list(box *p) {
    /* Convert a box to an R list.
     * 
     * Args:
     *   p: pointer to box to convert.
     * Returns:
     *   list containing: 
     *     'i' - indexes of points in the box (1-relative).
     *     'box' - coordinates of the corners of the box.
     */
    SEXP box_list, box_list_names, box_i, box_X, box_splits, tmp_splits, 
      box_matrix;

    PROTECT(box_list = allocVector(VECSXP, 3));

    PROTECT(box_list_names = allocVector(STRSXP, 3));
    SET_STRING_ELT(box_list_names, 0, mkChar("i"));
    SET_STRING_ELT(box_list_names, 1, mkChar("splits"));
    SET_STRING_ELT(box_list_names, 2, mkChar("box"));
    Rf_namesgets(box_list, box_list_names);
    UNPROTECT(1);

    /* Copy the indexes of the points in this box to box$i. */
    PROTECT(box_i = allocVector(INTSXP, p->points.n));
    for (int i = 0; i < p->points.n; i++) {
      INTEGER(box_i)[i] = p->points.i[i] + 1; /* Convert to 1-relative for R. */
    }
    SET_VECTOR_ELT(box_list, 0, box_i);
    UNPROTECT(1);
  
    /* Copy the splits that define this box to box$splits. */
    PROTECT(box_splits = allocVector(VECSXP, p->split->d));
    for (int i = 0; i < p->split->d; i++) {
      /* This could be changed to copy out "Left" and "Right" instead of 1
       *  and 2. 
       */
      PROTECT(tmp_splits = allocVector(INTSXP, p->split->nsplit[i]));
      for (int j = 0; j < p->split->nsplit[i]; j++) {
	INTEGER(tmp_splits)[j] = 
	  ((p->split->split[i] >> j) & 1) == LEFT_SPLIT ? 1 : 2;
      }
      SET_VECTOR_ELT(box_splits, i, tmp_splits);
    }
    SET_VECTOR_ELT(box_list, 1, box_splits);
    UNPROTECT(1 + p->split->d);
    
    /* Copy the lower-left and upper-right corners of this box to box$box. */
    PROTECT(box_matrix = allocMatrix(REALSXP, 2, p->split->d));
    double x1, x2;
    for (int j = 0; j < p->split->d; j++) {
      split_to_interval(p->split, j, &x1, &x2);
      REAL(box_matrix)[j * 2] = x1;
      REAL(box_matrix)[j * 2 + 1] = x2;
    }
    SET_VECTOR_ELT(box_list, 2, box_matrix);
    UNPROTECT(1);

    UNPROTECT(1);
    return box_list;
  }

  SEXP get_boxes_list(box **p, int n) {
    /* Convert an array of boxes into an R list of Boxes. 
     *
     * Args:
     *   p: array of box pointers.
     *   n: integer, length of array.
     * Returns:
     *   R list containing the converted boxes.
     */
    SEXP box_list;
    PROTECT(box_list = allocVector(VECSXP, n));
  
    for (int i = 0; i < n; i++) {
      SET_VECTOR_ELT(box_list, i, box_to_list(p[i]));
    }
  
    UNPROTECT(1);
    return box_list;
  }

  SEXP estimate_levelset(SEXP X, SEXP Y, SEXP k_max, SEXP gamma, SEXP delta,
			 SEXP rho) {
    /* Compute a levelset estimation. 
     *
     * Args:
     *   X: matrix of the X points, each row contains one point.  Columns 
     *      represent the different dimensions.
     *   Y: vector of the response variables.
     *   k_max: integer, maximum number of splits to consider.
     *   gamma: double, level of the level set.
     *   delta: double, complexity factor.
     *   rho: double, cost penalty.
     * Returns: levelset estimate.
     */
    /* Make sure that k_max, gamma, delta and rho are scalars. */
    if (LENGTH(k_max) != 1 || TYPEOF(k_max) != INTSXP) {
      error("k_max must be a single integer value.");
    }
    if (LENGTH(gamma) != 1 || TYPEOF(gamma) != REALSXP) {
      error("gamma must be a single numeric value.");
    }
    if (LENGTH(delta) != 1 || TYPEOF(delta) != REALSXP) {
      error("delta must be a single numeric value.");
    }
    if (LENGTH(rho) != 1 || TYPEOF(rho) != REALSXP) {
      error("rho must be a single numeric value.");
    }

    levelset_args la;
    SEXP dim;
    
    PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
    if (LENGTH(dim) != 2) {
      error("X must be a 2 dimensional matrix.");
    }

    la.n = INTEGER(dim)[0];
    la.d = INTEGER(dim)[1];
    UNPROTECT(1);
  
    if (TYPEOF(Y) != REALSXP || LENGTH(Y) != la.n) {
      error("Y must be a vector with length(Y) == dim(X)[1]");
    }
  
    /* Compute the levelset. */
    la.kmax  = INTEGER(k_max)[0];
    la.x     = REAL(X);
    la.y     = REAL(Y);
    la.gamma = REAL(gamma)[0];
    la.delta = REAL(delta)[0];
    la.rho   = REAL(rho)[0];

    /* Bucket everything up into a box collection. */
    levelset_estimate le = compute_levelset(points_to_boxes(la.x, la.n, la.d, 
							    la.kmax),
					    la);

    /* Make return list, has total cost, number of boxes left, a list for inset boxes, and a 
     * list for non-inset boxes. */
    SEXP ret, ret_names;
    PROTECT(ret = allocVector(VECSXP, 4));
    PROTECT(ret_names = allocVector(STRSXP, 4));

    SET_STRING_ELT(ret_names, 0, mkChar("total_cost"));
    SEXP total_cost;
    PROTECT(total_cost = Rf_allocVector(REALSXP, 1));
    REAL(total_cost)[0] = le.total_cost;
    SET_VECTOR_ELT(ret, 0, total_cost);
    UNPROTECT(1);
  
    SET_STRING_ELT(ret_names, 1, mkChar("num_boxes"));
    SEXP num_boxes;
    PROTECT(num_boxes = Rf_allocVector(REALSXP, 1));
    REAL(num_boxes)[0] = le.num_inset + le.num_non_inset;
    SET_VECTOR_ELT(ret, 1, num_boxes);
    UNPROTECT(1);
  
    SET_STRING_ELT(ret_names, 2, mkChar("inset_boxes"));
    SEXP inset_boxes;
    PROTECT(inset_boxes = get_boxes_list(le.inset_boxes, le.num_inset));
    SET_VECTOR_ELT(ret, 2, inset_boxes);
    UNPROTECT(1);
  
    SET_STRING_ELT(ret_names, 3, mkChar("non_inset_boxes"));
    SEXP non_inset_boxes;
    PROTECT(non_inset_boxes = get_boxes_list(le.non_inset_boxes, le.num_non_inset));
    SET_VECTOR_ELT(ret, 3, non_inset_boxes);
    UNPROTECT(1);
  
    Rf_namesgets(ret, ret_names);
    UNPROTECT(2);

    /* Cleanup. */
    for (int i = 0; i < le.num_inset; i++) {
      /* By definition, these boxes do not have children. */
      free_box_but_not_children(le.inset_boxes[i]);
    }
    free(le.inset_boxes);
  
    for (int i = 0; i < le.num_non_inset; i++) {
      /* By definition, these boxes do not have children. */
      free_box_but_not_children(le.non_inset_boxes[i]);
    }
    free(le.non_inset_boxes);
  
    return ret;
  }
}
