#include "street_table.h"

street_table* street_table_create(size_t size) {
  street_table* const table = malloc(sizeof *table);
  street_entry* const entries = malloc(size * sizeof *entries);
  table->entries = entries;
  table->size = size;
  table->count = 0;
  table->next_id = 0;
  table->sorting = st_none;

  return table;
}

street_table* street_table_add(street_table* table, const char* name, int* id) {
  *id = table->next_id++;

  table->entries[*id].name = strdup(name);
  table->entries[*id].id = *id;
  table->count += 1;

  return table;
}

static int street_name_comp(const void* a, const void* b) {
  const street_entry* const s1 = a;
  const street_entry* const s2 = b;
  return strcmp(s1->name, s2->name);
}

static int street_id_comp(const void* a, const void* b) {
  const street_entry* const s1 = a;
  const street_entry* const s2 = b;
  return s1->id < s2->id ? -1 : (s1->id > s2->id ? 1 : 0);
}

street_table* street_table_sort_ids(street_table* table) {
  const size_t size = table->count;
  qsort(table->entries, size, sizeof (street_entry), street_id_comp);
  table->sorting = st_ids;
  return table;
}

street_table* street_table_sort_names(street_table* table) {
  const size_t size = table->count;
  qsort(table->entries, size, sizeof (street_entry), street_name_comp);
  table->sorting = st_names;
  return table;
}

int street_table_find_id(street_table* table, const char* name) {
  if (table->sorting != st_names) {
    table = street_table_sort_names(table);
  }

  const size_t size = table->count;
  street_entry key = { .name = (char*) name, .id = -1, };
  street_entry* res = bsearch(&key, table->entries, size, sizeof (street_entry), street_name_comp);

  return res->id;
}

const char* street_table_find_name(street_table* table, int id) {
  if (table->sorting != st_ids) {
    table = street_table_sort_ids(table);
  }

  const size_t size = table->count;

  street_entry key = { .name = NULL, .id = id, };
  street_entry* res = bsearch(&key, table->entries, size, sizeof (street_entry), street_id_comp);

  return res->name;
}

