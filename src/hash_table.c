#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

void insert_element(hash_table *table, void *elem)
{
    int bucket = table->hash(elem) % table->buckets;
    if (table->count[bucket] < table->capacity[bucket])
    {
        table->elements[++(table->count[bucket])] = elem;
    }
    else
    {
        int cur_elements = table->count[bucket];
        void *new_bucket = calloc(table->capacity[bucket] * 2, sizeof(void*));
        memcpy(new_bucket, table->elements[bucket], cur_elements*sizeof(void*));
        table->capacity[bucket] *= 2;
        table->elements[++(table->count[bucket])] = elem;
    }
}

void *get_element(hash_table *table, void *key)
{

}

void init_table(hash_table *table, hash_func hash)
{
    table->buckets = 256;
    table->hash = hash;
    table->elements = calloc(table->buckets, sizeof(void*));
    table->capacity = calloc(table->buckets, sizeof(int));
    table->count = calloc(table->buckets, sizeof(int));
    for (int i = 0; i < table->buckets; i++)
    {
        table->elements[i] = calloc(10, sizeof(void*));
        table->capacity[i] = 10;
    }
}