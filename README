
Le fichier ./hashcode_2021_online_qualifications.pdf contient une description du sujet du concours de programmation Google Hashcode 2021 (phase de qualification). Le répertoire ./input/ contient les fichiers d'entrée. Note : les fichiers d'entrée qui s'appellent [a-f]long.txt ont été modifiés pour augmenter artificiellement la durée du calcul.

Les fichiers source de ce répertoire ont été écrits par l'équipe Bibeleskaes, composée de Harenome Razanajato, Cédric Bastoul et Vincent Loechner ((c) 2021). Ce code n'est pas forcément très joli à lire, mais il représente bien le type de code qu'un industriel pourrait vous demander de paralléliser, sans plus d'informations que cela (codes écrits dans des délais contraints, non documentés, pas très propres (!)). Le répertoire ./out/ contient des fichiers solution de chacun des problèmes à résoudre.

Un simple 'make' dans le répertoire principal va construire les deux exécutables './solver' et './checker'. Le checker fait une simulation de la circulation des voitures dans une ville composée de rues et de feux rouges, comme indiqué dans
le sujet. Exemple d'utilisation :
  $ ./checker input/d.txt out/d_best.out
  Score 1556815

Il peut être assez long à s'exécuter comme vous pouvez le constater, sur le fichier d'entrée d.txt notamment (ou pire sur le fichier dlong.txt). Le but de ce projet sera de l'accélérer, le plus possible, en utilisant à votre convenance toutes les techniques et toutes les ressources matérielles à votre disposition et que vous avez manipulées dans ce module de programmation parallèle.

La première difficulté de ce projet sera d'identifier les portions de code intéressantes à paralléliser : inutile de paralléliser une boucle d'initialisation de quelques itérations, vous ne gagnerez rien ! Vous ne pourrez pas paralléliser les lectures/écritures dans les fichiers d'entrée et de sortie non plus, vous les exécuterez en séquentiel. Vous êtes totalement libres de modifier, optimiser, paralléliser en OpenMP et en MPI l'ensemble du code qui se trouve entre ces entrées et sorties : les fonctions 'solution_check()' et
'solution_score()'. Nous attendrons de vous que vous obteniez le temps d'exécution le plus court possible pour cette partie de code, en utilisant toutes les ressources à votre disposition que vous souhaitez utiliser.

Vous rendrez :
- un rapport qui présente votre méthode de parallélisation de ce code, vos choix de techniques de parallélisation, les caractéristiques des machines que vous avez utilisées et, bien sûr, une analyse des performances obtenues ;
- les sources de votre projet (un fichier .tar.gz, inutile d'y remettre les gros répertoires input/ et out/)

Vous serez évalués selon trois critères :
- la correction de votre code parallèle : ça marche, ça respecte les normes OpenMP/MPI et c'est exécuté en parallèle,
- l'efficacité de votre code parallèle, s'il est correct bien sûr : vous avez utilisé de manière efficace les techniques de parallélisation adéquates, le temps d'exécution est grandement réduit,
- la beauté de votre rapport.
