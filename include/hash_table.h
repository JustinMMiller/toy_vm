#ifndef HASH_TABLE_H
#define HASH_TABLE_H
// Generated with python random.shuffle of range(0, 256)

typedef int(*hash_func)(void *);

typedef struct _hash_table
{
    hash_func hash;
    void **elements;
    // number of 1st level indices in elements
    int buckets;
    // how many entries are in each bucket
    int *count;
    // how many entries can each bucket hold
    int *capacity;
} hash_table;

void insert_element(hash_table *table, void *elem);
void *get_element(hash_table *table, void *key);
void init_table(hash_table *table, hash_func hash);

#endif