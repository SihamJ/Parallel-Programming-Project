#include <stdio.h>

#include "check.h"
#include "problem.h"
#include "solution.h"

void   solution_pprint_init(solution_t* s, problem_t *p)
{
	CHECK(s != NULL);
	CHECK(p != NULL);

        return;
}
// afficher le contenu du tableau
void   solution_pprint(solution_t* s, problem_t* p)
{
	CHECK(s != NULL);
	CHECK(p != NULL);

  printf("LIB (#books):");
  // for(int l=0; l < s->nb_libs; l++)
  // {
  //   printf(" %d (%d)", s->lib[l].lib_id, s->lib[l].count);
  // }
  printf("\n");
/*
  FILE *output = stderr;

  for( int l=0; l<t->H ; l++ )
  {
    for( int c=0 ; c<t->W ; c++ )
    {
      if(t->bitmap[l][c]==-1)
        fprintf(output, ".");
      else
        fprintf(output, "%c", 'A'+t->building[t->bitmap[l][c]].pnum );
    }
    fprintf(output, "\n");
  }
*/
}

void   solution_pprint_wait()
{
	char s[100];
	printf("> ");
	fflush(stdout);
	fgets(s,99,stdin);
}
