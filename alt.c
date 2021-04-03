#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"
#include "solve.h"

static int up[NB_LIBS_MAX];
static int scanned[NB_BOOKS_MAX];

void solve_init (solution_t* s, problem_t* p, char *initial_output_fname)
{
  // magic_fullness = 100.0;
  // magic_fitness = 30.0;
  // magic_needs = 1.0;
  // magic_sizes = 1.0;

  // 1 chance sur 2 de repartir d'un existant s'il est donné :)
  if(rand()%2 && initial_output_fname)
  {
    static solution_t initial_output_alloc;
    solution_t* initial_output = &initial_output_alloc;

    CHECK( solution_read(initial_output_fname, initial_output, p) == 0 );

    // TODO: créer la solution de départ 's' à partir de l'existante 'initial_output'

  }
  else {
    // TODO: initialiser la structure de sortie 's'
  }
}


void solve(solution_t* s, problem_t* p)
{
  const int nb_libs = p->nb_libs;
  int up_count = 0;

  while (up_count < nb_libs) {
    int min_i = INT_MAX;
    int min_time = INT_MAX;
    for (int i = 0; i < p->nb_libs; ++i) {
      int signup_days = p->lib[i].signup_days;
      if (!up[i] && signup_days < min_time) {
        min_i = i;
        min_time = signup_days;
      }
    }

    if (rand()%100 > 10)
    {
      s->lib[s->nb_libs].lib_id = min_i;
      up[min_i] = 1;

      const int nb_books = p->lib[min_i].count;
      int possible = 0;
      for (int i = 0; i < nb_books; ++i) {
        const int b = p->lib[min_i].books[i];
        if (!scanned[b]) {
          possible++;
        }
      }

      int scans = 0;
      while (scans < possible) {
        // for (int i = 0; i < nb_books; ++i) {
        //   const int b = p->lib[min_i].books[i];
        //   if (!scanned[b] && rand() % 100 > 75) {
        //     util_insert_book(&s->lib[s->nb_libs], b);
        //     scanned[b] = 1;
        //     scans++;
        //   }
        // }
        //
        int max_i = INT_MIN;
        int max_score = INT_MIN;

        for (int i = 0; i < nb_books; ++i) {
          const int b = p->lib[min_i].books[i];
          if (!scanned[b] && p->book[b] > max_score) {
            max_i = b;
            max_score = p->book[b];
          }
          // if (!scanned[b] && rand() % 100 > 75) {
          //   util_insert_book(&s->lib[s->nb_libs], b);
          //   scanned[b] = 1;
          //   scans++;
          // }
        }

        if (rand() % 100 > 50) {
          util_insert_book(&s->lib[s->nb_libs], max_i);
          scanned[max_i] = 1;
          scans++;
        }
      }

      s->nb_libs++;
      up_count++;
    }
  }
}

