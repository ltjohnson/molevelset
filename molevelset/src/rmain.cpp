#include "box.h"
#include "misc.h"
#include "findtree.h"

#include <R.h>
#include <Rinternals.h>

extern "C" {

SEXP molevelset(SEXP x, SEXP y, SEXP kmax, SEXP gamma, SEXP rho, SEXP delta) {
  SEXP result, resultNames, nSexp, dimSexp;
  int n, dim;
  
  n = LENGTH(y);
  dim = ncols(x);
  
  /* Create return list */
  PROTECT(result = allocVector(VECSXP, 2));
  PROTECT(resultNames = allocVector(STRSXP, 2));
  PROTECT(nSexp = allocVector(INTSXP, 1));
  INTEGER(nSexp)[0] = n;
  PROTECT(dimSexp = allocVector(INTSXP, 1));
  INTEGER(dimSexp)[0] = dim;
  
  SET_VECTOR_ELT(result, 0, nSexp);
  SET_VECTOR_ELT(result, 1, dimSexp);

  SET_STRING_ELT(resultNames, 0, mkChar("n"));
  SET_STRING_ELT(resultNames, 1, mkChar("dim"));
  namesgets(result, resultNames);
  
  UNPROTECT(4);
  
  return(result);
}

} // extern "C"
