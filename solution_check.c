#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include <mpi.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"

#ifdef DEBUG
#ifndef DEBUG_SCORE
    #define DEBUG_SCORE
  #endif
#endif


int solution_check(solution_t* const s, problem_t* const p) {
    /* OK: errors = 0. */
    int errors = 0;

//  const int nb_inter = p->NI;
    const int nb_streets = p->S;
    const int nb_inter_sol = s->A;


		// TO DO #pragma omp parallel for reduction(+:errors)
    for(int i=0; i<nb_inter_sol; i++)
    {
			int c = i;
        // vérifie la solution pour l'intersection num i : s->schedule[i]
        if(s->schedule[i].nb < 1)
        {
            fprintf(stderr, "intersection has no light (%d)\n", i);
        }

        for(int feu=0; feu<s->schedule[i].nb; feu++)
        {
            // s->schedule[i].t[feu] .rue et .duree sont valides
            const int rue = s->schedule[i].t[feu].rue;
            const char* const name = street_table_find_name(p->table, rue);
            if(rue >= nb_streets)
            {
                fprintf(stderr, "invalid street number (%d -> \"%s\")\n", rue, name);
                errors++;
					 	}
            int rid;

            // vérifie que cette rue (rue) arrive bien à cette intersection (i)
            for(rid=0; rid<nb_streets; rid++)
            {

                if(p->r[rid].street_id == rue)
										break;
  					}
            // p->r[rid] contient la rue, vérifie que la rue arrive bien à cette intersection
            if(p->r[rid].end != i)
            {
                fprintf(stderr, "invalid street number (%d -> \"%s\"): not arriving to the intersection %d\n", rue, name, i);
                errors++;
            }

            // durée > 0
            if(s->schedule[i].t[feu].duree <= 0)
            {
                fprintf(stderr, "invalid schedule length (intersection %d light %d -> %d)\n", i, feu, s->schedule[i].t[feu].duree);
            }
        }

    }

    /* OK */
    return errors;
}


typedef struct car_state {
    int street;     // Current street id,
    int distance;   // Remaining distance to end of street (0: end of street),
    int position;   // Position in the queue of the street (1: top position),
    int arrived;    // Arrived or not (Boolean),
    int nb_streets; // Number of streets already travelled
} car_state_t;

typedef struct street_state {
    int green;      // 1: green, 0: red
    int nb_cars;    // Number of cars in the street
    int max;        // Max number of cars in the street
    int out;        // A car just left the street (Boolean)
} street_state_t;


static car_state_t car_state[NB_CARS_MAX];
static street_state_t street_state[NB_STREETS_MAX];

void simulation_init(const problem_t* const p) {
    memset(car_state, 0, NB_CARS_MAX * sizeof(car_state_t));
    memset(street_state, 0, NB_STREETS_MAX * sizeof(street_state_t));

    // #p boucle d'initialisation, ne semble pas intéressante à paralléliser
    for (int i = 0; i < p->V; i++) {
        car_state[i].street = p->c[i].streets[0];
        car_state[i].distance = 0;
        // Queue the car
        street_state[car_state[i].street].nb_cars++;
        if (street_state[car_state[i].street].nb_cars > street_state[car_state[i].street].max)
            street_state[car_state[i].street].max = street_state[car_state[i].street].nb_cars;
        car_state[i].position = street_state[car_state[i].street].nb_cars;
        car_state[i].arrived = 0;
        car_state[i].nb_streets = 0;
    }
}

void simulation_update_intersection_lights(const solution_t* const s, int i, int T) {
    int cycle = 0;
    int tick = 0;
    int no_green_light = 1;

   #pragma omp parallel for reduction(+:cycle)
    // Find the light cycle total time
    for (int l = 0; l < s->schedule[i].nb; l++) {
        cycle += s->schedule[i].t[l].duree;
    }

    // Find at which time in the cycle we are
    tick = T % cycle;

    //printf("Inter %d, cycle %d, tick %d, T %d\n", i, cycle, tick, T);

    // Set the light state
		// TO DO : diminue les performances, pourquoi ?
	//	#pragma omp parallel for ordered shared(tick)
    for (int l = 0; l < s->schedule[i].nb; l++) {
        // Remove duration, if we get below zero, this light is green and others are red
			//	#pragma omp ordered
        tick -= s->schedule[i].t[l].duree;

        //printf("light %d, tick %d, duree %d\n", l, tick,  s->schedule[i].t[l].duree);
        if (tick < 0) {
            street_state[s->schedule[i].t[l].rue].green = 1;
            // #p si paralléliser alors atomic sur no_green_light
            no_green_light = 0;
            // #p pas la peine
            for (int next = l + 1; next < s->schedule[i].nb; next++) {
                street_state[s->schedule[i].t[next].rue].green = 0;
            }
						l = s->schedule[i].nb;
            //break;
        }
				if(l!=s->schedule[i].nb)
        	street_state[s->schedule[i].t[l].rue].green = 0;
    }

    if (no_green_light) {
        printf("PROBLEM: NO GREEN LIGHT AT INTERSECTION %d (cycle %d)\n", i, cycle);
    }
}

int simulation_update_car(const problem_t* const p, int c, int T) {
    // If already arrived, nothing to do
    if (car_state[c].arrived == 1){
			//fprintf(stderr, "T = %d , score returned 0\n",T );
        return 0;
			}
    // If at the end of street, light green, queue 1 then move to next street
    if ((car_state[c].distance == 0) &&
        (street_state[car_state[c].street].green == 1) &&
        (car_state[c].position == 1)) {
				//	fprintf(stderr, "T = %d , entered 1st condition\n",T );

        // Update number of street finished
        car_state[c].nb_streets++;
        // Signal a car left the street
        street_state[car_state[c].street].out = 1;
        // Set the new street where the car is
        car_state[c].street = p->c[c].streets[car_state[c].nb_streets];
        car_state[c].distance = p->r[car_state[c].street].len - 1;
        // Enqueue the car in the new street
        street_state[car_state[c].street].nb_cars++;
        if (street_state[car_state[c].street].nb_cars > street_state[car_state[c].street].max)
            street_state[car_state[c].street].max = street_state[car_state[c].street].nb_cars;
        car_state[c].position = street_state[car_state[c].street].nb_cars;
    } else if (car_state[c].distance > 0) {
			//fprintf(stderr, "T = %d , entered 1st else\n",T );
        // If not at the end of street, advance
        car_state[c].distance--;
    }

    // If now at the last street AND at the end of the street: drive complete!
    if ((car_state[c].street == p->c[c].streets[p->c[c].P - 1]) &&
        (car_state[c].distance == 0)) {
        car_state[c].arrived = 1;
        // Remove the car immediately from the street
        street_state[car_state[c].street].nb_cars--;

        #pragma omp parallel for
        // If another car is in that street and was there before that car, dequeue it
        for (int i = 0; i < p->V; i++) {
            if ((car_state[c].street == car_state[i].street) &&
                (car_state[c].position < car_state[i].position)) {
                car_state[i].position--;
            }
        }
				//fprintf(stderr, "T = %d , score returned !=0\n",T );
        return p->F + (p->D - (T + 1));
    }

    return 0;
}

void simulation_print_state(const problem_t* const p, int T) {
    printf("Timestep: %d\n", T);
    for (int c = 0; c < p->V; c++) {
        printf("Car %d -> street %d, distance: %d, position: %d, "
               "arrived: %d, street#: %d\n",
               c,
               car_state[c].street,
               car_state[c].distance,
               car_state[c].position,
               car_state[c].arrived,
               car_state[c].nb_streets);
    }
    for (int s = 0; s < p->S; s++) {
        printf("Street %d -> green: %d, nb_cars: %d, out: %d\n",
               s,
               street_state[s].green,
               street_state[s].nb_cars,
               street_state[s].out);
    }
}


void simulation_dequeue(const problem_t* const p) {

#pragma omp parallel for
        for (int street = 0; street < p->S; street++) {
            // If there is a street to dequeue
            if (street_state[street].out == 1) {
                // If a car is in that street, dequeue it
                for (int c = 0; c < p->V; c++) {
                    if (car_state[c].street == street) {
                        // pas besoin d'atomic, ne sera checké que par un seul thread
                        car_state[c].position--;
                    }
                }
                street_state[street].nb_cars--;
                street_state[street].out = 0;
            }
        }
    }


//#define DEBUG_SCORE

int simulation_run(const solution_t* const s, const problem_t* const p) {

    int rang, size, score = 0;

#ifdef DEBUG_SCORE
    problem_write(stdout, p);
  solution_write(stdout, s, p);
#endif

    // Init state
    simulation_init(p);

		if( MPI_Init(NULL, NULL))
		{
			fprintf(stderr, "Erreur MPI_Init\n");
			return(1);
		}
		MPI_Comm_rank( MPI_COMM_WORLD, &rang );
		MPI_Comm_size( MPI_COMM_WORLD, &size );

		int total_units = p->D;
		int units_per_process = p->D / size;
		int remaining_units = p->D % size;
		int total_processes = size;

		if(units_per_process == 0){
			units_per_process++;
			remaining_units = 0;
			total_processes = total_units;
		}

		// variables utiles dans le cas où le nombre de processus ne divise pas le nombre d'unités de temps, elles servent à partager les unités
		// restantes équitablement entre les processus.
		int bonus = rang < remaining_units;
		int bonus_prev = 0;

		if(rang != 0)
			for (int j = 0; j < remaining_units && j < rang; j++)
				bonus_prev++;

	// For each time step
	for (int T = rang*units_per_process + bonus_prev; T < total_units; T++) {

			#ifdef DEBUG_SCORE
						printf("Score: %d\n", score);
				printf("- 1 Init:\n");
				simulation_print_state(p, T);
			#endif

			// Attend de recevoir le résultat de street_state de la part du processus du rang qui le précède
			if(rang != 0  &&  T == rang*units_per_process + bonus_prev ){
				MPI_Status status;
			//	fprintf(stderr,"process %d receiving 0 from %d AND T = %d\n",rang,rang-1, T);
				MPI_Recv(&street_state, 4*NB_STREETS_MAX, MPI_INT, rang - 1, 0, MPI_COMM_WORLD, &status);
			}
			// Update light state for each intersection et l'envoie au processus de rang suivant si c'est son dernier tour de boucle

			#pragma omp parallel for
				for (int i = 0; i < s->A; i++)
					simulation_update_intersection_lights(s, i, T);

			if( rang != total_processes-1  &&  T == (rang+1)*units_per_process - 1 + bonus + bonus_prev){
				//fprintf(stderr,"process %d sending 0 AND T = %d\n",rang,T);
				MPI_Ssend(&street_state, 4*NB_STREETS_MAX, MPI_INT, rang + 1, 0, MPI_COMM_WORLD );
			}


			#ifdef DEBUG_SCORE
						printf("- 2 lights:\n");
				simulation_print_state(p, T);
			#endif


			if( rang > 0  &&  T == rang*units_per_process + bonus_prev){
				MPI_Status status1, status2;
				//fprintf(stderr,"process %d receiving 1 from %d AND T = %d\n",rang,rang-1,T);
				MPI_Recv(&street_state, 4*NB_STREETS_MAX, MPI_INT, rang - 1, 0, MPI_COMM_WORLD, &status1);
				//fprintf(stderr,"process %d receiving 2 from %d AND T = %d\n",rang,rang-1,T);
				MPI_Recv(&car_state, 5*NB_CARS_MAX, MPI_INT, rang - 1, 0, MPI_COMM_WORLD, &status2);
			}

				#pragma omp parallel for reduction(+:score)
				for (int c = 0; c < p->V; c++){
						score += simulation_update_car(p, c, T);
				//		fprintf(stderr,"process:%d temp score:%d AND T = %d\n",rang,score,T);
					}

						// Update car state

			#ifdef DEBUG_SCORE
						printf("- 3 cars (score now = %d):\n", score);
				simulation_print_state(p, T);
			#endif

			simulation_dequeue(p);

			if(  rang < total_processes-1  &&  T == (rang+1)*units_per_process + bonus_prev + bonus - 1 ){
				//fprintf(stderr,"process %d sending 1 AND T = %d\n",rang,T);
				MPI_Ssend(&street_state, 4*NB_STREETS_MAX, MPI_INT, rang + 1, 0, MPI_COMM_WORLD);
				//fprintf(stderr,"process %d sending 2 AND T = %d\n",rang,T);
				MPI_Ssend(&car_state, 5*NB_CARS_MAX, MPI_INT, rang + 1, 0, MPI_COMM_WORLD);
				//quitter la boucle
				T = total_units;
			}

			#ifdef DEBUG_SCORE
						printf("- 4 queues:\n");
				simulation_print_state(p, T);
			#endif
		}
	//	fprintf(stderr, "process %d reacher barrier\n",rang);
	//	fprintf(stderr,"process:%d final score:%d \n",rang,score);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allreduce(&score, &score, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		MPI_Finalize();
	//	fprintf(stderr,"process:%d reduced score:%d \n",rang,score);
		return score;
}


/*static int score_descending_order(void const* r1, void const* r2) {
  int d1 = ((int*)r1)[1];
  int d2 = ((int*)r2)[1];
  return (d1 == d2) ? 0 : ((d1 > d2) ? -1 : 1);
}*/

/*static int score_ascending_order(void const* r1, void const* r2) {
  int d1 = ((int*)r1)[1];
  int d2 = ((int*)r2)[1];
  return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
}*/

int tab_street[NB_STREETS_MAX][2];
int tab_car[NB_CARS_MAX][2];

int solution_score(solution_t* s, const problem_t* const p) {
    int score = 0;
    score = simulation_run(s, p);
    return score;
}
