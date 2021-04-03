#ifndef __PROBLEM_H__
#define __PROBLEM_H__

#include <stdio.h>
#include <stdbool.h>
#include "street_table.h"

// #define NB_BOOKS_MAX 100000
// #define NB_LIBS_MAX 30000
// // #define NB_LIBS_MAX 100000
// #define NB_DAYS_MAX 100000
//
// typedef struct plib
// {
//   int signup_days;    // entre 1 et 1,000
//   int books_per_day;  // entre 1 et 100,000
//   int s_lib_id;       // lib id dans solution
//
//   int count;  // nb books dans la lib (ceux déjà scannés sont virés)
//   int *books; // alloué dynamiquement: numéros des livres dans la bib.
//
//   // ceux qui restent après insertion dans les libs choisies...
//   int count_left;
//   int *books_left;  // p_bid, triés par score :)
// } plib_t; //

#define NB_INTER_MAX 100000
#define NB_STREETS_MAX 100000
#define NB_CARS_MAX 1000
#define NB_TRAVEL_MAX 1000

typedef struct rue
{
  int start, end;  // intersection num (début de rue/fin de rue)
  int street_id;   // hash
  int len;         // longueur de la rue.
} rue_t;

typedef struct car
{
  int P; // nb of streets
  int streets[NB_STREETS_MAX];
} car_t;

typedef struct problem
{
  int D; // durée de la simulation
  int NI; // nb intersections
  int S; // nb streets
  int V; // nb cars
  int F; // bonus points for each car that reaches its destination

  char hash[NB_STREETS_MAX][31];

  rue_t r[NB_STREETS_MAX]; // les rues (stockés en ordre arbitraire)

  car_t c[NB_CARS_MAX];
  street_table* table;
}
problem_t;

int    problem_read(char*, problem_t*);
void   problem_write(FILE*, problem_t*);
void problem_write_caracs(FILE* output, problem_t* p);

#endif /* __PROBLEM_H__ */
