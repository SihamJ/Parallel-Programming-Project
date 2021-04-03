#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"


void solution_write(FILE* output, solution_t* s, problem_t* p) {
  CHECK(p!=NULL);
  fprintf(output, "%d\n", s->A);

  p->table = street_table_sort_ids(p->table);
  for (int i = 0; i < s->A; i++) {
    if (s->schedule[i].nb > 0) {
      // ID
      fprintf(output, "%d\n", i);
      // Incomming streets covered
      fprintf(output, "%d\n", s->schedule[i].nb);

      // Name and duration for each light
      for (int j = 0; j < s->schedule[i].nb; ++j) {
        const int id = s->schedule[i].t[j].rue;
        const int duration = s->schedule[i].t[j].duree;
        const char* name = street_table_find_name(p->table, id);
        fprintf(output, "%s %d\n", name, duration);
      }
    } else {
      fprintf(stderr, "\033[33mWarning: schedule[%d].nb <= 0\033[0m\n", i);
    }
  }
}


// NOT_USED!
void solution_init(solution_t* s, problem_t* p) {
  CHECK(s != NULL);
  CHECK(p != NULL);
}

int solution_read(char* filename, solution_t* s, problem_t* p) {
  CHECK(s != NULL);
  CHECK(p != NULL);
  // static char buffer[UTIL_BUFFER_MAX + 1];
  FILE* input;

  if (filename == NULL) {
    input = stdin;
  }
  else {
    CHECK((input = fopen(filename, "r")) != NULL);
  }

  char buffer[4096];

  // A
  CHECK(fscanf(input, "%d", &s->A) == 1);
  for(int i=0 ; i<s->A ; i++)
  {
    // ID:
    int intersection = - 1;
    CHECK(fscanf(input, "%d", &intersection) == 1);

    int nb = -1;
    CHECK(fscanf(input, "%d", &nb) == 1);
    s->schedule[i].nb = nb;

    for (int j = 0; j < nb; ++j) {
      int duration = -1;
      CHECK(fscanf(input, "%s %d", buffer, &duration) == 2);
      int id = street_table_find_id(p->table, buffer);
      s->schedule[i].t[j].rue = id;
      s->schedule[i].t[j].duree = duration;
    }
  }

  if (filename != NULL)
    CHECK(fclose(input) != -1);

  return 0;
}
