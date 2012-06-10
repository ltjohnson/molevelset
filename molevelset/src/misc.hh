#ifndef MISC_H
#define MISC_H

#include "box.hh"

/* Count the number of splits in the given vector.  The number of
   splits is defined as the count of the first elements of the vector
   that are not STOP_SPLIT.  If all the elements are not STOP_SPLIT,
   this will give back the size of the vector. */
int BoxLength(vector<BoxSplit> splits);

/* For the given n points, x, y, where y is an array of length n, x is
   an array of length d*n in column-major format, find the collection
   of boxes in [0,1]^d that contains the points, using kmax splits in
   each direction. */
map<string, MOBox *> *FindBoxes(int n, int d, int kmax, double *x, double *y);

/* For the given univariate point x in [0, 1], using kmax splits, populate
   the vector of splits in *p with this box. */
void GetUnivariateSplits(double x, int kmax, vector<BoxSplit> *p);

/* For the given points x in [0, 1]^d, place them into the box *p
   using kmax splits in each dimension */
void GetBoxSplits(double *x, int kmax, int d, vector<vector<BoxSplit> > *p);

#endif
