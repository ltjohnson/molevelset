#include <R.h>
#include <Rinternals.h>

#include "box.h"

SEXP box_to_list(box *p, double *px, int n);

SEXP num_boxes(SEXP X, SEXP k_max) {
  SEXP ans, dim;
  box_collection *pc;
  
  PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
  if (LENGTH(dim) != 2) {
    error("X must be a 2 dimensional matrix.");
  }
  
  if (LENGTH(k_max) != 1) {
    error("k_max must be a scalar.");
  }
  
  pc = points_to_boxes(REAL(X), INTEGER(dim)[0], INTEGER(dim)[1], 
		       INTEGER(k_max)[0]);
  
  PROTECT(ans = allocVector(INTSXP, 1));
  INTEGER(ans)[0] = pc->n;
  
  free_collection(pc);

  UNPROTECT(2);
  return ans;
}

SEXP get_boxes(SEXP X, SEXP k_max) {
  SEXP ans, dim;
  box_collection *pc;
  
  PROTECT(dim = Rf_getAttrib(X, R_DimSymbol));
  if (LENGTH(dim) != 2) {
    error("X must be a 2 dimensional matrix.");
  }
  
  if (LENGTH(k_max) != 1) {
    error("k_max must be a scalar.");
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
  SEXP box_list, box_list_names, box_matrix, box_i, box_X;

  PROTECT(box_list = allocVector(VECSXP, 3));

  PROTECT(box_list_names = allocVector(STRSXP, 3));
  SET_STRING_ELT(box_list_names, 0, mkChar("i"));
  SET_STRING_ELT(box_list_names, 1, mkChar("X"));
  SET_STRING_ELT(box_list_names, 2, mkChar("box"));
  Rf_namesgets(box_list, box_list_names);
  UNPROTECT(1);

  PROTECT(box_i = allocVector(INTSXP, p->n));
  for (int i = 0; i < p->n; i++) {
    INTEGER(box_i)[i] = p->i[i] + 1; /* Convert to 1-relative for R. */
  }
  SET_VECTOR_ELT(box_list, 0, box_i);
  UNPROTECT(1);
  
  PROTECT(box_X = allocMatrix(REALSXP, p->n, p->split->d));
  for (int i = 0; i < p->n; i++) {
    for (int j = 0; j < p->split->d; j++) {
      REAL(box_X)[i + j * p->n] = px[p->i[i] + j * n];
    }
  }
  SET_VECTOR_ELT(box_list, 1, box_X);
  UNPROTECT(1);

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
