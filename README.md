# SATSolver
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216
Powered by git: https://github.com/Oxmose/SATSolver

## Arguments de la ligne de commande
./reso1 : Démmare l'interface utilisateur du programme tout est expliqué.

## Choix d'implémentation DPLL

L'architecture du projet a changé.  

L'implémentation générale de DPLL est similaire à la précédente mais a été allegée (gros gain de temps), on ne manipule plus que des structures assez légères se basant sur l'ID des clauses (on a une table m_clauses qui permet d'accéder à l'objet Clause avec l'ID).    
On fait les déductions à la volée avec une deductionQueue (avant on reparcourait tout pour spotter les déductions).    
Le preprocess est maintenant mis en évidence.  
m_valuation joue un rôle beaucoup plus central qu'avant, c'est par exemple dans les WL notre seul manière de savoir si un litéral est vivant ou pas.  

Nous avons choisi d'exploiter un paradigme de stratégies.  
C'est à dire que toutes les portions de codes qui dépendent du choix de l'utilisateur
sont virtualisées et implémentées par héritage, typiquement les choix de stratégies de bets et les watch literals.  

Sur la forme cela permet d'avoir un code plus concis, plus clair.  
Sur le fond le changement est aussi justifié, un exemple.  
Sans les watch literals, on peut maintenir pour chaque clause la liste de ses variables vivantes mais avec watch literals ça tue l'idée de la méthode,
on ne veut rien faire sur litéraux non watched. Mais dans le cas standard c'est pertinent. On implémente donc les deux variantes et cela est facilité par l'utilisation des stratégies.    

Tous ces détails d'implémentations relatifs à chaque méthode sont donc écrits dans SATSolverSTD (standard) et SATSolverWL, sachant qu'ils ont toujours accès aux fonctions qu'ils partagent implémentées dans SATSolver.    

Cela permet par exemple (pour l'instant) d'avoir une fonction solve() ou deduce() générique.   
Les fonctions virtuelles pures sont celles qui représentent les changement à implémenter dans chaque variante de DPLL.  
Idem pour les bets et les deux parsers.    

Pour la polarisation unique on veut dans tous les cas, quelque soit la méthode, qu'elle existe pour le prétraitement c'est le but de l'implémentation dans SATSolver.cpp, on la désactive pour les WL avec le flag m_isWL. SATSolverSTD.cpp possède une spécialisation car on peut tirer profit de ses structures propres pour aller plus vite dans ce cas. Le flag m_isWL relève d'un détail d'implémentation (on ne peut pas facilement utiliser l'héritage dans ce cas) que l'on peut détailler si nécessaire.    

Les backtracks de STD et WL diffèrent de très peu (dans le cas STD on met à jour m_aliveVarsIn) mais on le laisse virtuel pur (donc à implémenter) car il avait été dit que les backtracks allaient changer dans les prochains rendus.   

On ne renumérote pas les variables comme proposé dans le sujet supposant que l'utilisateur rentre une entrée qui lui convient. Et même, si l'utilisateur veut juste faire un test sur une grosse en supprimant des variables pour debuguer son fichier, ce n'est pas pratique pour lui si notre solver renumérote tout.    

### Améliorations:         

- On pourrait tirer profit des structures de la variante standard pour aller plus vite dans les bets (là on doit toujours parcourir tous les litéraux et vérifier leur affectation).   


## Générateur de formules CNF
Afin de générer une formule CNF, il suffit d'éxécuter le programme sans aucuns arguments. Un menu vous sera alors présenté et vous aurez le choix de générer une formule SAT CNF. Pour ce faire suivez les instructions.       


## Performances
###Optimisation à la compilation  
Tous les tests présentés ont été fait en -O3. Cela améliore énormément les performances.
Un exemple de comparaison sur aim-100-1_6-no-1.cnf sans WL et standard bet:  


### Base de test CNF (cf: dev/bin/test_base/first_set/cnf)
Sans flag -O3 : 14.652s

Avec flag -O3 : 1.977s 

###Structure du répertoire de test, bin/test_base

* cnf/for : différents formats
* cnf/first_set : les dummy tests présentés au rendu 1
* cnf/corr_test : nos tests de correction pour tester les nouvelles mise à jour du code
* cnf/time_test : nos tests de performance

###Courbes

Voir doc/courbes/ (sur le github très certainement soumission un peu tardive)

###Gain en performance par raport au rendu 1

Un petit exemple qui compare le rendu1 avec le meilleur résultat du rendu2 sur un exemple : 

#### dubois22.cnf rendu 1
UNSATISFIABLE

Temps d'éxécution : 3m50.931s

#### dubois22.cnf rendu 2 en WL standard Bet
UNSATISFIABLE

Temps d'éxécution : 1m9.022s

#### dubois22.cnf rendu 2 en CL standard Bet
//TODO

#### dubois22.cnf rendu 2 en WL etCL standard Bet
//TODO

### Perfomances du rendu 2

## Watched literals
On gagne en moyenne 20% de temps par rapport au moment ou les watched literals ne sont pas activés, cependant il nous arrive de perdre du temps sur certaines formules.

## Heuristiques
L'heuristique la plus efficace en moyenne est -moms souvent avec -wl.   
-rand donne souvent de bonnes surprises.


### La clause de l'horreur

dev/bin/test_base/cnf/first_set/bf0432-007.cnf

Pas re-testée depuis le rendu1.


## Qui a fait quoi
###Tristan
* rendu1 : Implémentation DPLL, prétraitement, optimisation avec liste de priorité, structures de données et tests.
* rendu2 : Refonte DPLL, implémentation Watched Literals, redesign en statégies, scripts de tests, optimisation.

###Alexy
* rendu1 : Parser CNF, prétraitement, transformation de Tseitin, structures de données, interface, générateur de formules et tests.
* rendu2 : Heuristiques de paris, redesign en statégies, modifications du parser, scripts de tests, optimisation.
* rendu3 : Recherche de l'UIP dans le graph, création de la coupe UIP, heuristiques, interface, scripts de test, corrections de bugs, optimisation.

# Scripts de test

Deux bateries de tests sont disponibles :
##Correction
Dans bin

test_correction.sh permet de vérifier que le resultat renvoyé par le solveur est correcte. Pour ce faire, lancer le, les tests sont éfféctués sur les CNF dans le dossier bin/test_base/cnf/corr_test.

Vous pourrez y mettre les CNF que vous voulez en prenant bien soins de mettre les SAT dans le dossier sat et les UNSAT dans le dossier unsat.

##Temps
Dans bin

test_total_time.sh permet de vérifier que les temps de résolution pour les heuristique ou non, watched literals activé ou non sur les CNF.

Les tests sont éfféctués sur les CNF dans le dossier bin/test_base/cnf/serial_test.

Vous pourrez y mettre les CNF que vous voullez en prenant bien soins de mettre les SAT dans le dossier sat et les UNSAT dans le dossier unsat.

# Rendu 3
## Ce qui a été fait
* Implémentation des CL (avec et sans WL)
* Implémentation de l'exploration de conflits interactif
* Implémentation des heuristiques FORGET et VSIDS
* Correction de bugs (Tseitin et prétraitement)
* Nouveaux set de tests (correction et temps)
* Nouveaux scripts de test (correction et temps)
* Nouveau makefile, plus pratique à utiliser (ne recompile que les fichiers modifiés), faire un make clean, make à chaque pull du git
## Qui a fait quoi
* Se référer au chapitre "Qui a fait quoi" plus haut.

## Performances
* Les heuristiques RAND et RAND0 se révèlent être dans le meilleur des cas très bonne avec le CL.

## Le retour de la clause de l'horreur
Lors des premiers tests nous n'étions pas parvenu à résoudre la formule :

dev/bin/test_base/cnf/first_set/bf0432-007.cnf

Or à l'aide de CL et WL avec l'heuristique VSIDS, nous pouvons la résoudre en 0.001s.

## Retour sur les algorithmes de détéction de UIP
### Naif
Le premier algorithme implémenté pour la détéction du premier UIP se déroule en temps quadratique. Pour chaque noeuds dans le graph, on retire le noeud. On parcour ensuite le graph du noeud de conflit. Si l'on atteind la racine alors ce noeud n'est pas un UIP. Une fois tous les UIP détéctés, on choisi le premier en faisant un BFS à partir du conflit et pour chaque noeuds rencontrés, on vérifi s'il est dans la liste des UIP. Au premier UIP rencontré on le retourne.
### Linéraire
Le second algorithme se déroule en temps linéaire en le nombre de noeuds et d'arrêtes du graphe.
* Dans un premier temps, on produit le graphe inverse (G-1).
* Puis parcours BFS à partir du dernier BET, on recréé alors un graph de ce BET vers le noeud de conflit en syétrisant les arrêtes.
* On applique un algorithme de détéction de points d'articulations dans ce nouveau graph.
* On parcours le graph G-1 à partir du noeuds de conflict, le premier noeud rencontré étant un noeud d'articulation dans le graph symétrisé est le First UIP.
