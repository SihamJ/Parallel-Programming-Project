#ifndef _STREET_TABLE_H
#define _STREET_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum street_table_sorting_type {
  st_none = 0,
  st_ids,
  st_names,
} street_table_sorting_type;

typedef struct street_entry {
  char* name;
  int id;
} street_entry;

typedef struct street_table {
  struct street_entry* entries;
  size_t size;
  size_t count;
  int next_id;
  street_table_sorting_type sorting;
} street_table;

street_table* street_table_create(size_t size);
street_table* street_table_add(street_table* table, const char* name, int* id);

street_table* street_table_sort_ids(street_table* table);
street_table* street_table_sort_names(street_table* table);
int street_table_find_id(street_table* table, const char* name);
const char* street_table_find_name(street_table* table, int id);

street_table* street_table_destroy(street_table* t);

#endif /* _STREET_TABLE_H */
