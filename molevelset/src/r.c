#include <R.h>
#include <Rinternals.h>

#include "box.h"
#include "molevelset.h"

SEXP box_to_list(box *p, double *px, int n);

SEXP get_boxes(SEXP X, SEXP k_max) {
  SEXP ans, dim;
  box_collection *pc;
  
  PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
  if (LENGTH(dim) != 2) {
    error("X must be a 2 dimensional matrix.");
  }
  
  if (LENGTH(k_max) != 1 || TYPEOF(k_max) != INTSXP) {
    error("k_max must be a scalar integer.");
  }
  
  pc = points_to_boxes(REAL(X), INTEGER(dim)[0], INTEGER(dim)[1], 
		       INTEGER(k_max)[0]);
  
  /* Copy boxes out to a list. */
  PROTECT(ans = allocVector(VECSXP, pc->n));
  box_node *p_node = pc->boxes;
  for (int i = 0; i < pc->n; i++) {
    SET_VECTOR_ELT(ans, i, box_to_list(p_node->p, REAL(X), INTEGER(dim)[1]));
    p_node = p_node->next;
  }
  
  free_collection(pc);

  UNPROTECT(2);

  return ans;
}

SEXP box_to_list(box *p, double *px, int n) {
  /* Convert a box to an R list.
   * 
   * Args:
   *   p: pointer to box to convert.
   *   px: pointer to coordinates of the points in the box.
   *   n: total number of points.
   * Returns:
   *   list containing: 
   *     'i' - indexes of points in the box (1-relative).
   *     'X' - matrix of coordinates of points in the box.
   *     'box' - coordinates of the corners of the box.
   */
  SEXP box_list, box_list_names, box_i, box_X, box_splits, tmp_splits, 
    box_matrix;

  PROTECT(box_list = allocVector(VECSXP, 4));

  PROTECT(box_list_names = allocVector(STRSXP, 4));
  SET_STRING_ELT(box_list_names, 0, mkChar("i"));
  SET_STRING_ELT(box_list_names, 1, mkChar("X"));
  SET_STRING_ELT(box_list_names, 2, mkChar("splits"));
  SET_STRING_ELT(box_list_names, 3, mkChar("box"));
  Rf_namesgets(box_list, box_list_names);
  UNPROTECT(1);

  /* Copy the indexes of the points in this box to box$i. */
  PROTECT(box_i = allocVector(INTSXP, p->n));
  for (int i = 0; i < p->n; i++) {
    INTEGER(box_i)[i] = p->i[i] + 1; /* Convert to 1-relative for R. */
  }
  SET_VECTOR_ELT(box_list, 0, box_i);
  UNPROTECT(1);
  
  /* Copy the points in this box to box$X. */
  PROTECT(box_X = allocMatrix(REALSXP, p->n, p->split->d));
  for (int i = 0; i < p->n; i++) {
    for (int j = 0; j < p->split->d; j++) {
      REAL(box_X)[i + j * p->n] = px[p->i[i] + j * n];
    }
  }
  SET_VECTOR_ELT(box_list, 1, box_X);
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
  SET_VECTOR_ELT(box_list, 2, box_splits);
  UNPROTECT(1 + p->split->d);
  
  /* Copy the lower-left and upper-right corners of this box to box$box. */
  PROTECT(box_matrix = allocMatrix(REALSXP, 2, p->split->d));
  double x1, x2;
  for (int j = 0; j < p->split->d; j++) {
    split_to_interval(p->split, j, &x1, &x2);
    REAL(box_matrix)[j * 2] = x1;
    REAL(box_matrix)[j * 2 + 1] = x2;
  }
  SET_VECTOR_ELT(box_list, 3, box_matrix);
  UNPROTECT(1);

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
   * Returnns: levelset estimate.
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

  int n, d;
  SEXP dim;
  
  PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
  if (LENGTH(dim) != 2) {
    error("X must be a 2 dimensional matrix.");
  }
  n = INTEGER(dim)[0];
  d = INTEGER(dim)[1];
  UNPROTECT(1);
  
  if (TYPEOF(Y) != REALSXP || LENGTH(Y) != n) {
    error("Y must be a vector with length(y) == dim(X)[1]");
  }
  
  /* Bucket everything up into a box collection. */
  box_collection *pc = points_to_boxes(REAL(X), n, d, INTEGER(k_max)[0]);
  
  /* Compute the levelset. */
  box_collection *plevelset = 
    compute_levelset(pc, REAL(Y), n, d, INTEGER(k_max)[0], REAL(gamma)[0], 
		     REAL(delta)[0], REAL(rho)[0]);
  
  /* Convert the final level set estimation to R usable return value. */
  
  /* Cleanup. */
  
  return NULL;
}
