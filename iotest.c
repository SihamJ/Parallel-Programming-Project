#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"
#include "solve.h"

static problem_t  p;
static solution_t s;

int main(int argc, char* argv[])
{
  if (argc <= 1)
  {
    fprintf(stderr, "Usage: %s problem.in [solution.out]\n", argv[0]);
    return -1;
  }

  printf("---------------INPUT---------------\n");
  CHECK(problem_read (argv[1],  &p) == 0);
  problem_write(stdout, &p);

  if(argc==2)
  {
    // si pas de sortie fournie, affiche les stats du problème
    printf("---------------STATS---------------\n");
    problem_write_caracs(stdout, &p);
  }
  else if(argc==3)
  {
    printf("---------------OUTPUT---------------\n");
    CHECK(solution_read (argv[2], &s, &p) == 0);
    solution_write(stdout, &s, &p);
  }
  else
  {
    fprintf(stderr, "Usage: %s problem.in [solution.out]\n", argv[0]);
  }

  return EXIT_SUCCESS;
}
