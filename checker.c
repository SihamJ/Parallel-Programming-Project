#include <stdio.h>
#include <stdlib.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"

static problem_t  p;
static solution_t s;

int main(int argc, char* argv[])
{
  int score;
struct timeval tv_begin, tv_end;
	gettimeofday( &tv_begin, NULL);

  if (argc != 3) {
    fprintf(stderr, "usage: %s problem solution\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  CHECK(problem_read(argv[1], &p) == 0);
  CHECK(solution_read(argv[2], &s, &p) == 0);

  CHECK(solution_check(&s, &p) == 0);
  score = solution_score(&s, &p);
  gettimeofday( &tv_end, NULL);

  fprintf(stderr, "Score %d\n", score);
	fprintf(stderr, "Temps: %lfs\n", DIFFTEMPS(tv_begin, tv_end));

  // Write the score file
  util_write_score(argv[2], score);

  return(0);
}
