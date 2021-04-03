#ifndef __SOLUTION_H__
#define __SOLUTION_H__

#include <stdbool.h>
#include <tgmath.h>

#include "problem.h"

#define NB_INCOMING_MAX 1000
typedef struct sched
{
  // ordre et durée de chaque feu
  int nb;

  struct rue_duree_t {int rue, duree;} t[NB_INCOMING_MAX];
} sched_t;

typedef struct solution
{
  int refine; // = 1 : une solution à rafiner, on ne part pas de rien.
  int A; // nb intersections in the sol (s->A = p->NI)
  sched_t schedule[NB_INTER_MAX]; // stockés dans l'ordre des intersections
}
solution_t;

void   solution_init(solution_t*, problem_t* p);
int    solution_read(char*, solution_t*, problem_t* p);
void   solution_write(FILE*, solution_t*, problem_t*);
int    solution_check(solution_t*, problem_t*);
int solution_score(solution_t*, const problem_t*);


/* ces trois fonctions se trouvent dans :
- consoleprint.c pour la version texte
// - graphprint.c pour la version graphique
*/
void   solution_pprint_init(solution_t* s, problem_t *);
void   solution_pprint(solution_t *, problem_t *);
void   solution_pprint_wait();

#endif /* __SOLUTION_H__ */
