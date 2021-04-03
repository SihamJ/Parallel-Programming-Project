#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"
#include "solve.h"

int nb_input = 6;
char* input[] = {
  "input/a_example.txt",
  "input/b_read_on.txt",
  "input/c_incunabula.txt",
  "input/d_tough_choices.txt",
  "input/e_so_many_books.txt",
  "input/f_libraries_of_the_world.txt",
};

static problem_t  p;
static solution_t s;

int main(int argc, char* argv[]) {
  char output_file_name[UTIL_BUFFER_MAX];
  FILE* output = NULL;

  srand(time(0) + getpid());

  if (argc > 4) {
    fprintf(stderr, "usage: %s input_file [output_file] [initial output to refine]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 1) {
    int global_score = 0;

    for (int i = 0; i < nb_input; i++) {
      int score;
      //UTIL_msg_begin("Reading input problem: ");
      CHECK(problem_read(input[i], &p) == 0);
      //UTIL_msg_end("ok");
      solve_init(&s, &p, NULL);

      solve(&s, &p);
      CHECK(solution_check(&s, &p) == 0);
      score = solution_score(&s, &p);
      global_score += score;

      fprintf(stderr, "Score %34s: %8d\n", input[i], score);

      // Print the output
      snprintf(output_file_name, UTIL_BUFFER_MAX, "%s.out", input[i]);
      CHECK((output = fopen(output_file_name, "w")) != NULL);
      solution_write(output, &s, &p);
      CHECK(fclose(output) == 0);

      // Print the score
      util_write_score(output_file_name, score);
    }
    fprintf(stderr, "Score %34s: %8d\n", "global", global_score);
  } else {
    int score;
    //UTIL_msg_begin("Reading input problem: ");
    CHECK(problem_read(argv[1], &p) == 0);
    //UTIL_msg_end("ok");

    solve_init(&s, &p, (argc == 4)?argv[3]:NULL);

    solve(&s, &p);
    CHECK(solution_check(&s, &p) == 0);
    score = solution_score(&s, &p);

    fprintf(stderr, "Score %34s: %8d\n", argv[1], score);

    // Print the output
    if (argc == 2) {
      snprintf(output_file_name, UTIL_BUFFER_MAX, "%s.out", argv[1]);
    } else {
      snprintf(output_file_name, UTIL_BUFFER_MAX, "%s", argv[2]);
    }
    CHECK((output = fopen(output_file_name, "w")) != NULL);
    solution_write(output, &s, &p);
    CHECK(fclose(output) == 0);

    // Print the score
    util_write_score(output_file_name, score);
  } // argc==1

  return EXIT_SUCCESS;
}
