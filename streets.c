#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char* street_names[] = { 
  "rue 0",
  "rue 1",
  "rue 2",
  "rue 3",
  "rue 8",
  "rue 4",
  "rue 6",
  "rue 5",
  "rue 7",
  "rue 9",
};

typedef struct street {
  char* name;
  size_t id;
} street;

static int street_name_comp(const void* a, const void* b) {
  const street* const s1 = a;
  const street* const s2 = b;
  return strcmp(s1->name, s2->name);
}

static int street_id_comp(const void* a, const void* b) {
  const street* const s1 = a;
  const street* const s2 = b;
  return s1->id < s2->id ? -1 : (s1->id > s2->id ? 1 : 0);
}

struct street streets[10000];

int main(int argc, char* argv[argc + 1]) {
  for (size_t i = 0; i < 10; ++i) {
    streets[i].name = strdup(street_names[i]);
    streets[i].id = i;
    printf("entering streets[\"%s\"] = %zu\n", streets[i].name, streets[i].id);
  }

  qsort(streets, 10, sizeof (street), street_name_comp);
  for (size_t i = 0; i < 10; ++i) {
    printf("streets[\"%s\"] = %zu\n", streets[i].name, streets[i].id);
  }

  printf("\n\n");

  street key;
  key.name = street_names[2];
  street* res = bsearch(&key, streets, 10, sizeof (street), street_name_comp);
  if (res)
    printf("found streets[\"%s\"] = %zu\n", res->name, res->id);
  else
    printf("????\n");
  fflush(stdout);

  printf("\n\n");

  qsort(streets, 10, sizeof (street), street_id_comp);
  for (size_t i = 0; i < 10; ++i) {
    printf("streets[\"%s\"] = %zu\n", streets[i].name, streets[i].id);
  }

  printf("\n\n");

  key.name = NULL;
  key.id = 8;
  res = bsearch(&key, streets, 10, sizeof (street), street_id_comp);
  printf("found streets[%zu] = \"%s\"\n", res->id, res->name);

  printf("\n\n");

  return 0;
}
