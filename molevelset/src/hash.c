#include <stdlib.h>
#include <string.h>

#include "hash.h"

hash * make_hash(int size, int (*mk_key)(void *),
		 int (*cmp)(void *k1, void *k2)) {
  hash *h = (hash *)malloc(sizeof(hash));

  h->arr_size      = size;
  h->cmp           = cmp;
  h->make_hash_key = mk_key;

  h->arr  = (hash_entry **)malloc(sizeof(hash_entry*) * h->arr_size);
  
  memset( h->arr, 0, sizeof(hash_entry*) * h->arr_size);
  
  return( h );
}

hash_entry *hash_find(hash *h, void *key) {
  hash_entry *p;
  int hkey = h->make_hash_key( key );
  hkey = hkey % h->arr_size;
  
  p = h->arr[hkey];
  while(p) {
    if (h->cmp(p->key, key) == 0)
      break;
    p = p->next;
  }
  return( p );

}

void hash_del(hash *h, void *key) {
  hash_entry *p, *parent;
  int i;
  int hkey = h->make_hash_key( key );
  hkey = hkey % h->arr_size;

  parent = NULL;
  p = h->arr[hkey];
  while(p) {
    if (h->cmp(p->key, key) == 0)
      break;
    parent = p;
    p = p->next;
  }
  if (!p)
    return;
  
  // delete p, move p->next to parent->next.
  if (parent = NULL)
    h->arr[hkey] = p->next;
  else
    parent->next = p->next;
  
  free(p);

}

hash_entry *hash_add(hash *h, void *key, void *data) {
  int hkey;
  hash_entry *p;

  hkey = h->make_hash_key( key ) % h->arr_size;
  
  p = (hash_entry *)malloc(sizeof(hash_entry));
  p->key = key;
  p->data = data;
  
  p->next = h->arr[hkey];
  h->arr[hkey] = p;

  return( p );
}
