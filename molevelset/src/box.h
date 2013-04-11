#ifndef BOX_H
#define BOX_H

#include <vector>
#include <string>
#include <map>

using namespace std;

/* Using left = 1 and right = 2 for compatibility with the reference paper.
   Adding stop = 0 for convenience. */
enum BoxSplit { STOP_SPLIT = 0, LEFT_SPLIT = 1, RIGHT_SPLIT = 2};

/* Convenience structs.  Several MOBox functions return a double, int pair */
typedef struct { double risk; int inset; } MORisk;
typedef struct { double cost; int inset; } MOCost;

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
    vector<int> pointsIndex, checked;
    vector<MOBox *> children;
    double *py, *px;
    double A;
    int dim, kmax, n, nbox, terminalNode;
    vector< vector<BoxSplit> > splits;
  public:
    /* Args:
     *   kmax: maximum numbe of splits.
     *   dim:  dimension of x.
     *   n: number of points.
     *   px: pointer to x points, array of length n*d, column-major format.
     *   py: pointer to y points, array of length n.
     *   splits: vector of vector of BoxSplit.  Gives the splits for 
     *     this box.
     */
    MOBox(int kmax, int dim, int n, double *px, double *py, 
          vector< vector<BoxSplit> > splits);

    /* Adds the point with index i to this box. */
    void AddPoint(int i);
    void AddPoint(vector<int>);
    /* Return vector of point indicies. */
    vector<int> GetPoints();

    /* Check whether the collapsing the split in dimension i has been
       checked. */
    int GetChecked(int);
    /* Mark dimension i as checked. */
    void SetChecked(int);

    /* Compute the complexity of this box, up to the probability 1-delta. */
    double Phi(double delta);
    /* Compute the empirical risk for this box for the level gamma. */
    MORisk Risk(double gamma);
    /* Compute the cost of this box, using level gamma, probability
       1-delta, and complexity penalty rho. */
    MOCost Cost(double gamma, double delta, double rho);
    
    /* Add a children to this box, order doesn't matter */
    void SetChildren(MOBox *, MOBox *);
    vector<MOBox *> GetChildren();

    string GetBoxKey();
    static string GetBoxKey(vector< vector<BoxSplit> >);
    string GetSiblingKey(int);
    vector<vector<BoxSplit> > GetSiblingSplits(int);
    string GetParentKey(int);
  
    /* Create the parent box containing this box by dropping split i */
    MOBox *CreateParentBox(int);
};

#endif