# Rapport de Projet OpenMP/MPI
L3 informatique

**Siham Janati**
**Abdelheq Delmi Bouras**

## Environnement

Le système utilisé pour réaliser les tests et les mesures de performances est :

```
OS : Linux sihartist-ROG-Strix-G531GT-G531GT 5.4.0-52-generic #57~18.04.1-Ubuntu SMP Thu Oct 15 14:04:49 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
OS dans une machine physique
#processeurs: 12
intel hyper-threading : Thread(s) per core:  2
compilateur(s) :
- gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
```

## HASHCODE

### Explications

Le but de ce projet est de paralléliser un code pour améliorer ses performances, en utilisant OpenMP et MPI dans les sections qui nous intéressent.
Ce code prends en entrée le plan urbain d'une ville et les chemins planifiés de toutes ses voitures, et doit renvoyer un planning optimale pour les feux d'intersections pour permettre à un nombre maximum de voiture de finir leurs trajets, c'est suivant ce critère que le score final est calculé.


### Etude du code et définition des sections à paralléliser

Nous nous intéressons dans ce projet au fichier *checker.c* uniquement, notamment les deux fonctions *solution_check* et *solution_score*.
La fonction *solution_check* vérifie que la solution proposé est valide, la fonction *solution_score* calcule le score total obtenu, la partie la plus importante du travail portera sur cette dernière.

***Solution_check***
Cette fonction vérifie que les données fournies dans le fichier solution correspondent bien au plan de la ville et aux chemins définie dans le fichier problème.
Le code boucle sur toutes les intersections une par une et vérifie que les informations de chacune correspondent. Si tout se passe bien, le code de retour est 0, sinon le nombre d'erreurs trouvés.  

***Solution_score***
Cette fonction ne fait qu'appeler *simulation_run* qui fait toute l'exécution et renvoie le score. On aurait pu supprimer *solution_score* et ne garder que *simulation_run*, tout le code de *solution_score* ne s'exécute pas à part l'appel à *simulation_run*.
*simulation_run* commence par une initialisation des feux d'intersections et de la positions des voitures, ensuite elle met à jour ces derniers après chaque passage d'une unité de temps. C'est ce que fait la plus grande boucle dans cette fonction. Pour partager équitablement la charge entre tous les processus et maximiser les performances, il serait plus judicieux d'utiliser MPI sur cette boucle et OpenMP sur les boucles à l'intérieur de cette dernière. Il y a par contre des dépendances entre les itérations qu'il faudra prendre en considération.
Nous expliquerons notre parallélisation plus en détails dans les sections suivantes.

### Parallélisation MPI  

Ci-dessous un schéma représentant la parallélisation de *simulation_score* avec MPI:  

![Schéma](schema.png)


Les feux de circulations sont indépendants de la position de la voiture et ne dépendent que de T, il y a donc moyen de calculer l'état des feux pendant que le processus d'avant calcule l'état des voitures.
Par contre, après le calcul de l'état des voitures, on modifie à nouveau les intersections sauf que on ne modifie pas les mêmes attributs. Au départ, on modifie les feux, mais à la fin de l'itération on modifie le nombre de voiture présentes dans l'intersection. Si le processus suivant reçoit ses modifications, il va écraser l'état des feux qu'il à mis à jour, il est nécessaire donc de recevoir les nouvelles modifications dans une nouvelle variable, puis de copier les nouvelles valeurs du nombre de voiture uniquement sans toucher à l'état des feux (ensemble du code plus bas):
```
if( total_processes > 1 && T != 0){
	MPI_Status status;
	MPI_Recv(&temp, 4*NB_STREETS_MAX, MPI_INT, (rang - 1)%total_processes, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&car_state, 5*NB_CARS_MAX, MPI_INT, (rang - 1)%total_processes, 0, MPI_COMM_WORLD, &status);

	#pragma omp parallel for
	for(int i=0; i<NB_STREETS_MAX; i++){
		street_state[i].nb_cars = temp[i].nb_cars;
		street_state[i].out = temp[i].out;
	}
}
```



#### code central

```
for(int T = rang; T < rang + total_processes*(units_per_process+bonus); T += total_processes ){

	#pragma omp parallel for
	for (int i = 0; i < s->A; i++)
			simulation_update_intersection_lights(s, i, T);

	// recv prev info car & lights, & update lights
	street_state_t temp[NB_STREETS_MAX];

	if( total_processes > 1 && T != 0){
		MPI_Status status;
		MPI_Recv(&temp, 4*NB_STREETS_MAX, MPI_INT, (rang - 1)%total_processes, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&car_state, 5*NB_CARS_MAX, MPI_INT, (rang - 1)%total_processes, 0, MPI_COMM_WORLD, &status);

		#pragma omp parallel for
		for(int i=0; i<NB_STREETS_MAX; i++){
			street_state[i].nb_cars = temp[i].nb_cars;
			street_state[i].out = temp[i].out;
		}
	}

	// calculate info car
	#pragma omp parallel for reduction(+:score)
	for (int c = 0; c < p->V; c++){
			score += simulation_update_car(p, c, T);
		}

		simulation_dequeue(p);

	// send info car & lights
	if( total_processes > 1 && T != total_units-1){
		MPI_Ssend(&street_state, 4*NB_STREETS_MAX, MPI_INT, (rang + 1)%total_processes, 0, MPI_COMM_WORLD);
		MPI_Ssend(&car_state, 5*NB_CARS_MAX, MPI_INT, (rang + 1)%total_processes, 0, MPI_COMM_WORLD);
	}

}

MPI_Allreduce(&score, &score, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
```

### Parallélisation OpenMP

#### I- parallélisation sans dépendances:



*simulation_dequeue*

#### code

```
```

*simulation_update_car*

#### code

```
```

#### II- parallélisation avec dépendances:

<!--Explications-->

*solution_check*

#### code

```
```

*simulation_update_intersection_lights*

#### code

```
```


### Mesures de performances

##### EN LOCAL SUR MA MACHINE



##### TURING SUR PLUSIEURS MACHINES


### Analyse des résultats et conclusion
