#IFNDEF HASH_H
#define HASH_H

typedef struct hash_entry {
  void *data;
  void *key;
  struct hash_entry *next;
} hash_entry;

typedef struct hash {
  hash_entry **arr;
  int arr_size;
  int (*make_hash_key)(void *key);
  int (*cmp)(void *key1, void *key2);
} hash;

hash *make_hash(int size, int (*mk_key)(void *), 
		int (*cmp)(void *k1, void *k2));
hash_entry *hash_find(hash *h, void *key);
void hash_del(hash *h, void *key);
hash_entry *hash_add(hash *h, void *key, void *data);

#endif
