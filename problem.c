#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "check.h"
#include "util.h"
#include "problem.h"

int problem_read(char* filename, problem_t* p) {
  static char buffer[UTIL_BUFFER_MAX + 1];
  FILE* input;

  if (filename == NULL) {
    input = stdin;
  }
  else {
    CHECK((input = fopen(filename, "r")) != NULL);
  }

  // Global parameters:
  CHECK(fgets(buffer, UTIL_BUFFER_MAX, input) != NULL);
  CHECK(buffer[strlen(buffer) - 1] == '\n');
  CHECK(sscanf(buffer, "%d %d %d %d %d", &p->D, &p->NI, &p->S, &p->V, &p->F) == 5);

  p->table = street_table_create(NB_STREETS_MAX);

  char street_buffer[2048];
  for (int i = 0; i < p->S ; i++) {
    struct rue* r = &p->r[i];
    CHECK( fscanf(input, "%d %d %s %d", &r->start, &r->end, street_buffer, &r->len) == 4);
    int id = -1;
    p->table = street_table_add(p->table, street_buffer, &id);
    r->street_id = id;
  }

  p->table = street_table_sort_names(p->table);
  for(int i=0 ; i<p->V ; i++)
  {
    int path_size = 0;
    CHECK(fscanf(input, "%d", &path_size) == 1);

    p->c[i].P = path_size;
    for (int j = 0; j < path_size; ++j) {
      CHECK(fscanf(input, "%s", street_buffer) == 1);
      int id = street_table_find_id(p->table, street_buffer);
      p->c[i].streets[j] = id;
    }
  }

  if (filename != NULL)
    CHECK(fclose(input) != -1);

  return 0;
}

// à faire à la fin (pas utilisé sauf débug)
void problem_write(FILE* output, problem_t* p) {
  (void) output; (void) p;

  // D I S V F
  // Duration, Intersections, Streets, Cars, Bonus
  fprintf(output, "%d %d %d %d %d\n", p->D, p->NI, p->S, p->V, p->F);

  // Start, End, Name, Length
  for (int i = 0; i < p->S; ++i) {
    const struct rue* const r = &p->r[i];
    const int id = r->street_id;
    const char* name = street_table_find_name(p->table, id);
    // Start, End, Name, Length
    fprintf(output, "%d %d %s (%d) %d\n", r->start, r->end, name, id, r->len);
  }

  // Count, [streets...]
  for (int i = 0; i < p->V; ++i) {
    const int path_size = p->c[i].P;
    fprintf(output, "%d ", path_size);
    for (int j = 0; j < path_size; ++j) {
      const int id = p->c[i].streets[j];
      const char* name = street_table_find_name(p->table, id);
      fprintf(output, "%s (%d) ", name, id);
    }
    fprintf(output, "\n");
  }
}

// à faire à la fin (pas utilisé sauf débug)
void problem_write_caracs(FILE* output, problem_t* p) {
  (void) output; (void) p;
  // fprintf(output, "nb days: %d\n", p->nb_days);

  // int min_score, max_score;
  // min_score = max_score = p->book[0];
  // for(int b = 0; b < p->nb_books; b++) {
  //   min_score = MIN(min_score,p->book[b]);
  //   max_score = MAX(max_score,p->book[b]);
  // }
  // fprintf(output, "BOOKS:\n");
  // fprintf(output, "  count: %d\n", p->nb_books);
  // fprintf(output, "  min score: %d\n", min_score);
  // fprintf(output, "  max score: %d\n", max_score);

  // fprintf(output, "LIBS:\n");
  // fprintf(output, "  nb libs: %d\n", p->nb_libs);
  // // fprintf(output, "  min nb books in lib: %d\n", );
  // // fprintf(output, "  min nb books in lib: %d\n", );

  // int count[2] = {p->lib[0].count,p->lib[0].count}; // min/max
  // int signup[3] = {p->lib[0].signup_days,p->lib[0].signup_days,0}; // min/max/total
  // int bpd[2] = {p->lib[0].books_per_day,p->lib[0].books_per_day}; // min/max
  // for(int l=0 ; l<p->nb_libs ; l++)
  // {
  //   count[0] = MIN(count[0],p->lib[l].count);
  //   count[1] = MAX(count[1],p->lib[l].count);
  //   signup[0] = MIN(signup[0],p->lib[l].signup_days);
  //   signup[1] = MAX(signup[1],p->lib[l].signup_days);
  //   signup[2] += p->lib[l].signup_days;
  //   bpd[0] = MIN(bpd[0],p->lib[l].books_per_day);
  //   bpd[1] = MAX(bpd[1],p->lib[l].books_per_day);
  // }
  // fprintf(output, "  nb books in [%d, %d]\n", count[0], count[1]);
  // fprintf(output, "  signup days in [%d, %d], total: %d\n", signup[0], signup[1], signup[2]);
  // fprintf(output, "  books per day in [%d, %d]\n", bpd[0], bpd[1]);
}
