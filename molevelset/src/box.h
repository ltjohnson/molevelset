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

typedef struct {
  box_node *boxes;  /* Linked list of boxes. */
  int n;            /* Number of boxes. */
  int d;            /* Number of dimensions. */
} box_collection;

box_collection *points_to_boxes(double *px, int n, int d, int k_max);

/* Functions for working with collections. */
void add_box(box_collection *, box *);
box * find_box(box_collection *, unsigned int *splits, int *nsplits);

unsigned int point_to_split(double *px, int d, int k_max);
void point_to_box(double *px, int d, int k_max, unsigned int *pbox);

/* Functions for working with boxes. */
void free_box(box *);
box *new_box(int d, unsigned int *splits, int *nsplits, int size_guess);
void add_point(box *, int);

#endif
