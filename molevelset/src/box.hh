#ifndef BOX_H
#define BOX_H

#include <vector>
#include <string>
#include <map>

using namespace std;

/* Using left = 1 and right = 2 for compatibility with the reference paper.
   Adding stop = 0 for convenience. */
enum BoxSplit { STOP_SPLIT = 0, LEFT_SPLIT = 1, RIGHT_SPLIT = 2};
int BoxLength(vector<BoxSplit>);

/* Convenience structs.  Several MOBox functions return a double, int pair */
typedef struct { double risk; int inset } MORisk;
typedef struct { double cost; int inset } MOCost;

/***************************************************************************
 * Class to manage boxes in the [0,1]^d cube.  
 * 
 * Boxes contain between 0 and n points.  These points have x values in 
 * [0,1]^d and real y values.  Boxes are formed by bisecting [0,1] up to 
 * kmax times in each of the d dimensions, and chosing either the left or
 * right splits (halves).  Thus [0,1]^d may be divided into at most 
 * 2**(d * kmax) boxes, or as few as 1 boxes (no splits).  For example, if 
 * d=2, and kmax=2, the box with corners at the points (0.25, 0.75) and 
 * (0.5, 1.0) would be described by the splits [ [LEFT_SPLIT, RIGHT_SPLIT], 
 * [RIGHT_SPLIT, RIGHT_SPLIT]]
 */
class MOBox {
  private:
    vector<int> pointsIndex;
    double *py, **px;
    int d, kmax, n, nbox;
    vector< vector<BoxSplit> > splits;
  public:
    MOBox(int kmax, int dim, int n, double **px, double *py, 
          vector< vector<BoxSplit> > splits);

    void AddPoint(int i);

    double Phi(double delta);
    MORisk Risk(double gamma, double A);
    MOCost Cost(double gamma, double delta, double rho, double A);
    
    static string GetBoxKey(vector< vector<BoxSplit> >);
};

/* For the given n points, x, y, where y is a n-by-1 array and x is a
   n-by-d array, find the collection of boxes in [0,1]^d that contains the
   points, using kmax splits in each direction. */
map<string, MOBox> FindBoxes(int n, int d, int kmax, double **x, double **y);

#endif
