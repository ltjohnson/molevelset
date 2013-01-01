#ifndef box_h
#define box_h

#define MAX_SPLITS 31 
#define LEFT_SPLIT 0
#define RIGHT_SPLIT 1

typedef struct {
  int d;                 /* Number of dimensions */
  int *nsplits;          /* Number of splits in each direction. */
  unsigned int *splits;  /* Splits in each direction. */
  int n;                 /* Number of points in this box. */
  int *i;                /* Indexes of the points. */
  int isize;             /* Size of i array. */
} box;

typedef struct box_node {
  box *p;
  struct box_node *next;
} box_node;

/* Box collections are used at each level of the level set finding
 * algorithm.  Boxes would best be stored in a hash, but AFAIK there are no
 * good GPL hash implementations in C available.  We could probably use
 * something in STL in C++, but I haven't investigated how hard that is to
 * do in a cross-os R library.  Instead, we use a linked list.  Linked
 * lists are probably fast enough, most of the time there won't be that
 * many boxes (max of 2^(k_max * d)) so a linear search is probably
 * sufficient.
 */
typedef struct {
  box_node *boxes;  /* Linked list of boxes. */
  int n;            /* Number of boxes. */
  int d;            /* Number of dimensions. */
} box_collection;

box_collection *points_to_boxes(double *px, int n, int d, int k_max);

/* Functions for working with collections. */
int add_box(box_collection *, box *);
box *find_box(box_collection *, unsigned int *splits, int *nsplits);
void free_collection(box_collection *);

unsigned int point_to_split(double *px, int d, int k_max);
void split_to_interval(unsigned int split, int nsplit, double *x1, double *x2);
void point_to_box(double *px, int d, int k_max, unsigned int *pbox);
int compare_splits(int d, unsigned int *s1, int *ns1, unsigned int *s2, int *ns2);

/* Functions for working with boxes. */
void free_box(box *);
box *new_box(int d, unsigned int *splits, int *nsplits, int size_guess);
void add_point(box *, int);

#endif
