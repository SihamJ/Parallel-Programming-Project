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

void refine(solution_t* s, problem_t* p);

//-----------------------------------------------------------------------------
// appelée une fois avant solve
void solve_init (solution_t* s, problem_t* p, char *initial_output_fname)
{
  CHECK(s != NULL);
  CHECK(p != NULL);

  // 9 chances sur 10 de repartir d'un existant s'il est donné :)
  if(rand()%10!=0 && initial_output_fname)
  {
    static solution_t initial_output_alloc;
    solution_t* initial_output = &initial_output_alloc;

    CHECK( solution_read(initial_output_fname, initial_output, p) == 0 );

    // TODO: créer la solution de départ 's' à partir de l'existante 'initial_output'
    s->A = initial_output->A;
    s->refine = 1;

    for(int i=0; i<s->A; i++)
    {
      s->schedule[i].nb = initial_output->schedule[i].nb;
      for(int j=0; j<initial_output->schedule[i].nb; j++)
      {
        s->schedule[i].t[j] = initial_output->schedule[i].t[j];
      }
    }
  }
  else {
    // TODO: initialiser la structure de sortie 's'
    s->refine = 0;

    // met tous les feux rouges dans la solution
    s->A = p->NI;
    for(int i=0; i<s->A; i++)
    {
      s->schedule[i].nb = 0;
    }

    // parcourt toutes les rues et les met dans le schedule de l'intersection
    // avec un délai de 1 :)
    for(int i=0; i<p->S; i++)
    {
      // p->r[i] - la rue

      // num de l'intersection d'arrivée :
      int end = p->r[i].end;

      // ajoute cette rue à s->schedule[end]
      int nb = s->schedule[end].nb;
      s->schedule[end].t[nb].rue = p->r[i].street_id;
      s->schedule[end].t[nb].duree = 1; // TODO: tester 1+rand()%3;
      s->schedule[end].nb ++;
      if(s->schedule[end].nb>=NB_INCOMING_MAX)
      {
        fprintf(stderr,"please increase NB_INCOMING_MAX\n");
        exit(1);
      }
//      printf("ajout de la rue %d au schedule de %d\n", p->r[i].street_id, end);
    }

  }
}

//-----------------------------------------------------------------------------

int desc(const void* p, const void* q) {
  int* l = (int*)p;
  int* r = (int*)q;
  return (r[1] - l[1]);
}

int asc(const void* p, const void* q) {
  int* l = (int*)p;
  int* r = (int*)q;
  return (l[1] - r[1]);
}

void print_tab(int* tab, int size) {
  for (int i = 0; i < 2*size; i+=2)
    fprintf(stderr, "%d ", tab[i]);
  fprintf(stderr, "\n");
  for (int i = 1; i < 2*size; i+=2)
    fprintf(stderr, "%d ", tab[i]);
  fprintf(stderr, "\n");
}

int Gcd(int a, int b)
{
  if(b==0)
    return a;
  else
    return Gcd(b, a%b);
}

void post_traite_sched(sched_t *s)
{
  // POST TRAITEMENT : supprimer les routes avec des zéros
  for(int j=0; j<s->nb; j++)
  {
    if(s->t[j].duree==0 && s->nb>1)
    {
      // diminue nb de 1
      s->nb--;
      // et le remplace par le dernier :
      s->t[j] = s->t[s->nb];
      // recommence avec le même j (beurk)
      j--;
    }
  }
  const int nb = s->nb;
  if(nb==1)
    s->t[0].duree=1;

  // // POST TRAITEMENT : diviser les schedules par leur gcd si applicable.
  int gcd = s->t[0].duree;
  for(int j=1; j<nb; j++)
  {
    gcd = Gcd(gcd, s->t[j].duree);
  }
  if(gcd!=1)
    for(int j=0; j<nb; j++)
      s->t[j].duree /= gcd;


  // // si la somme des durées est trop grande faut-il la réduire ?
  // int somme_durees=0;
  // for(int j=0; j<nb; j++)
  //   somme_durees += s->t[j].duree;

  // TODO ?
}

void melange_sched(sched_t *s)
{
  const int nb = s->nb;

  // mélange complètement les routes
  // en mode bourrin : tire une position aléatoire et fait l'échange de chaque feu
  for(int j=0; j<nb; j++)
  {
    int nouv = rand()%nb;
    struct rue_duree_t backup = s->t[j];
    s->t[j] = s->t[nouv];
    s->t[nouv] = backup;
  }

}

void solve(solution_t* s, problem_t* p)
{
  // je rafine une solution existante :
  if(s->refine)
  {
    refine(s, p);
    return;
  }

  // sinon, j'en crée une nouvelle :

  // calcule le nombre de voiture qui entre dans chaque rue.
  unsigned int nb_cars_on_street[NB_STREETS_MAX];
  for(int i=0; i<p->S ; i++)
  {
    nb_cars_on_street[i] = 0;
  }
  for(int car=0 ; car<p->V ; car++)
  {
    // p->c[car]
    for(int street=0 ; street<p->c[car].P ; street++)
    {
      // p->c[car].streets[street]
      const int st = p->c[car].streets[street];
      nb_cars_on_street[st] ++;
    }
  }

  // RANDOMISATION sur les voitures entrantes : 50%
  for(int i=0; i<p->S ; i++)
  {
    if(nb_cars_on_street[i]>10000000)
      fprintf(stderr,"risque de débordement entier (randomisation)\n");
    // multiplie le nombre de voitures entrantes par un nombre aléatoire entre 100 et 200 :)
    nb_cars_on_street[i] *= 100 + rand()%100;
  }

  // met dans le schedule une valeur qui dépend du nombre de voitures dans chaque rue qui y entre.
  // plus il y a de voitures dans la rue plus ce nombre est grand (en proportion du nombre total
  // de voitures entrantes)
  // au moins égal à 1 s'il y a une voiture qui peut entrer.
  for(int i=0; i<s->A; i++)
  {
    unsigned int voitures_entrantes = 1; // éviter la division par 0
    for(int j=0; j<s->schedule[i].nb; j++)
    {
      int rue = s->schedule[i].t[j].rue;
      voitures_entrantes += nb_cars_on_street[rue];
    }

    unsigned int facteur = 5; // rapport de 1 à (1+facteur) max (entier) entre les différents feux
    if(voitures_entrantes<10)
      facteur = 0; // il n'y a pas beaucoup de voitures, on fait un cycle de 1 pour chaque feu actif

    for(int j=0; j<s->schedule[i].nb; j++)
    {
      int rue = s->schedule[i].t[j].rue;
      // nb_cars_on_street[rue];
      if(nb_cars_on_street[rue] == 0)
      {
        s->schedule[i].t[j].duree = 0;
        // il faudra supprimer cette ligne
      }
      else
      {
        s->schedule[i].t[j].duree = 1 + facteur*nb_cars_on_street[rue]/voitures_entrantes;
      }
    }

    post_traite_sched(&s->schedule[i]);
    melange_sched(&s->schedule[i]);

  }
}

// proba d'échanger un feu avec son suivant
#define PROBA_ECHANGE_FEU 200
// une chance sur x de modifier un schedule existant (de +/-1)
#define PROBA_MODIFIE_SCHED 20

void modifie_sched(sched_t *s, const int only_add)
{
  const int nb = s->nb;

  if(only_add)
  {
    if(rand()%2)
    {
      // agressif: quelques durées *2 +1
      for(int j=0; j<nb ; j++)
      {
        const int x = rand()%(PROBA_MODIFIE_SCHED);
        if(x==0)
          s->t[j].duree = s->t[j].duree * 2 + 1;
      }
    }
    else
    {
      // ajoute 1 à quelques durées :
      for(int j=0; j<nb ; j++)
      {
        const int x = rand()%(PROBA_MODIFIE_SCHED);
        if(x==0)
          s->t[j].duree ++;
      }
    }
    return;
  }


  // échange quelques routes avec la suivante
  // en mode bourrin : échange aléatoire avec suivant
  for(int j=0; j<nb-1 ; j++)
  {
    if(rand()%PROBA_ECHANGE_FEU == 0)
    {
      struct rue_duree_t backup = s->t[j];
      s->t[j] = s->t[j+1];
      s->t[j+1] = backup;
    }
  }

  // modifie quelques durées (une chance sur PROBA_MODIFIE_SCHED)
  if(rand()%2)
  {
    // agressif: quelques durées /2 (peut supprimer des feux)
    for(int j=0; j<nb ; j++)
    {
      const int x = rand()%(PROBA_MODIFIE_SCHED);
      if(x==0)
        s->t[j].duree /= 2;
    }
  }
  else
  {
    // quelques durées +/- 1
    for(int j=0; j<nb ; j++)
    {
      const int x = rand()%(2*PROBA_MODIFIE_SCHED);
      if(x==0)
        s->t[j].duree ++;
      // // ne marche pas bien, supprime trop de feux :
      // else if(x==1)
      else if(x==1 && s->t[j].duree>1)
        s->t[j].duree --;
    }
  }

}

void refine(solution_t* s, problem_t* p)
{
  // part d'une solution existante et la rafine

  // version 1 :
  // une chance sur deux de réordonner aléatoirement les feux d'une interstion sur 10, sans aucune autre modif :
  if(rand()%2)
  {
    for(int i=0; i<s->A; i++)
      if(rand()%10==0)
        melange_sched(&s->schedule[i]);
    return;
  }

  // version 2 : bourrin
  // parcourt tous les schedules et les modifie légèrement (un sur 10)
  // une chance sur deux de ne faire que des add aux délais.
  const int only_add = rand()%2;
  for(int i=0; i<s->A; i++)
  {
    if(rand()%10==0)
    {
      modifie_sched(&s->schedule[i], only_add);
      post_traite_sched(&s->schedule[i]);
    }
  }


}
