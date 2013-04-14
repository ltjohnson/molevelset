#ifndef box_h
#define box_h

#define MAX_SPLITS 31 
#define LEFT_SPLIT 0
#define RIGHT_SPLIT 1
#define BOX_SUCCESS 1
#define BOX_ERROR 0

#define KEY_UNSIGNED_LONG_LONG 1
#define KEY_STRING 2

typedef struct {
  int d;             /* Number of dimensions. */
  int kmax;          /* Max number of splits in a dimension. */
  int key_hash_type; /* Indicates the data type used for the key hash. */
} box_split_info;

typedef struct {
  int *nsplit;          /* Number of splits in each direction. */
  unsigned int *split;  /* Split in each direction. */
  void *key;            /* Pointer to key hash for this split. */
} box_split;

typedef struct {
  int calculated;       /* Indicates if the risk components for this box
                           have been calculated. */
  int inset;            /* Is this box inside the set. */
  double inset_risk;    /* The inset risk for this box. */    
  double cost;          /* The cost for this box. */
  double risk_cost;     /* risk + cost for this box. */
} box_risk;

typedef struct {
  int n;             /* Number of points in this box. */
  int *i;            /* Indexes of the points. */
  int isize;         /* Size of i array. */
} box_points;

typedef struct box {
  box_split *split;  /* Split for this box. */
  int *checked;      /* Has collapsing a split in this dimension been
			checked. */
  box_points points; /* Points for this box. */
  int terminal_box;  /* Is this a terminal box, or does it have children
			boxes?. */
  struct box *children[2];  /* Children boxes. */
  box_risk risk;     /* Contains the risk information for this box. */
} box;

typedef struct box_node {
  box *p;
  struct box_node *next;
} box_node;

/* Box collections are used at each level of the level set finding
 * algorithm.  Boxes would best be stored in a hash, but AFAIK there are no
 * good GPL hash implementations in C available.  We could probably use
 * something from STL in C++, but I haven't investigated how hard that is
 * to do in a cross-platform R library.  Instead, we use a linked list.
 * Linked lists are probably fast enough, most of the time there won't be
 * that many boxes (max of 2^(k_max * d)) so a linear search is probably
 * sufficient.  Patches welcome.
 */
typedef struct {
  void *h;              /* Pointer to hash of boxes. */
  box_split_info *info; /* Pointer to boxs plit info. */
} box_collection;

box_collection *points_to_boxes(double *px, int n, int d, int k_max);

/* Functions for working with collections. */
box_collection *new_box_collection(box_split_info *);
int add_box(box_collection *, box *);
int remove_box(box_collection *, box_split *split);
box *find_box(box_collection *, box_split *split);
box *find_box_sibling(box_collection *, box_split *, int);
void free_collection(box_collection *);
box *get_first_box(box_collection *);
int num_boxes_in_collection(box_collection *);
box **get_terminal_boxes(box *);
box **list_boxes(box_collection *src);

int split_to_interval(box_split *split, int dim, double *x1, double *x2);
int compare_splits(box_split *ps1, box_split *ps2);

/* Functions for working with box_splits. */
box_split * copy_box_split(box_split *split);
int remove_split(box_split *split, int dim);
int copy_box_split2(box_split *to, box_split *from);
void free_box_split(box_split *split);
box_split *new_box_split(int d);
void *new_box_split_key(box_split *, box_split_info *);
void free_box_split_key(void *, box_split_info *);

/* Box split info functions. */
int box_split_key_type(int d, int kmax);
box_split_info *new_box_split_info(int d, int kmax);
void free_box_split_info(box_split_info *);

/* Functions for working with boxes. */
void free_box_but_not_children(box *);
box *new_box(box_split *split, int size_guess);
void add_point(box *, int);
box *copy_box(box *);

#endif
