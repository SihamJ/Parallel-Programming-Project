#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
//#include <mpi.h>
#include <omp.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"

#define DIFFTEMPS(a,b) (((b).tv_sec - (a).tv_sec) + ((b).tv_usec - (a).tv_usec)/1000000.)

static problem_t  p;
static solution_t s;

int main(int argc, char* argv[])
{


  int score;
	struct timeval tv_begin, tv_end;

  if (argc != 3) {
    fprintf(stderr, "usage: %s problem solution\n", argv[0]);
    exit(EXIT_FAILURE);
  }


	CHECK(problem_read(argv[1], &p) == 0);
	CHECK(solution_read(argv[2], &s, &p) == 0);
	CHECK(solution_check(&s, &p) == 0);
	gettimeofday( &tv_begin, NULL);

	/*if( MPI_Init(&argc, &argv))
	{
		fprintf(stderr, "Erreur MPI_Init\n");
		return(1);
	}
	MPI_Comm_rank( MPI_COMM_WORLD, &rang );
	MPI_Comm_size( MPI_COMM_WORLD, &size );*/
  score = solution_score(&s, &p);
	// fprintf(stderr, "rang: %d, score: %d\n",rang,score );


//	MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday( &tv_end, NULL);


  	fprintf(stderr, "Score %d\n", score);
		fprintf(stderr, "Temps d'éxecution: %lfs\n", DIFFTEMPS(tv_begin, tv_end));
  	// Write the score file
  	util_write_score(argv[2], score);
	

//	MPI_Finalize();
  return(0);
}
