#ifndef box_h
#define box_h

#include <map>
#include <string>

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
  int d;                /* Number of dimensions. */
} box_split;

class BoxSplitKey {
 private:
  unsigned long long lkey;
  std::string skey;
  int key_hash_type;
  
  void SetSplitULL(box_split *, box_split_info *);
  void SetSplitString(box_split *, box_split_info *);
  
 public:
  BoxSplitKey();
  BoxSplitKey(box_split *, box_split_info *);
  void SetSplit(box_split *, box_split_info *);
  
  void print_key() const;
  
  bool operator<(const BoxSplitKey &right) const;
};


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


/* Box collections are used at each level of the level set finding
 * algorithm.  We hash them by the split.  Fast searching is
 * important.  We need to be able to find sibling boxes quickly. */
typedef struct {
  std::map<BoxSplitKey,box *> *h; /* Pointer to map of boxes in collection. */
  box_split_info *info;           /* Pointer to box split info for
				     this collection. */
} box_collection;

box_collection *points_to_boxes(double *px, int n, int d, int k_max);

/* Functions for working with collections. */
box_collection *new_box_collection(box_split_info *);
int add_box(box_collection *, box *);
int remove_box(box_collection *, box_split *split);
int box_collection_size(box_collection *);
box *find_box(box_collection *, box_split *split);
box *find_box_sibling(box_collection *, box_split *, int);
void free_collection(box_collection *);
box *get_first_box(box_collection *);
box **list_boxes(box_collection *src);

int split_to_interval(box_split *split, int dim, double *x1, double *x2);
int compare_splits(box_split *ps1, box_split *ps2);

/* Functions for working with box_splits. */
box_split * copy_box_split(box_split *split);
int remove_split(box_split *split, int dim);
int copy_box_split2(box_split *to, box_split *from);
void free_box_split(box_split *split);
box_split *new_box_split(int d);

/* Box split info functions. */
int box_split_key_hash_type(int d, int kmax);
box_split_info *new_box_split_info(int d, int kmax);
void free_box_split_info(box_split_info *);

/* Functions for working with boxes. */
void free_box_but_not_children(box *);
box *new_box(box_split *, int);
void add_point(box *, int);
box *copy_box(box *);
box **get_terminal_boxes(box *);

/* Output functions, only used for debugging.  */
void print_box(box *);
void print_collection(box_collection *);
void print_split(box_split *s);

#endif
