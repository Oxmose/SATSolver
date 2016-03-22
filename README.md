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

Nous avons choisi d'exploiter un paradigme de stratégies.  
C'est à dire que toutes les portions de codes qui dépendent du choix de l'utilisateur
sont virtualisées et implémentées par héritage, typiquement les choix de stratégies de bets et les watch literals.  

Sur la forme cela permet d'avoir un code plus concis, plus clair.  
Sur le fond le changement est aussi jsutifié, un exemple.  
Sans les watch literals, on peut maintenir pour chaque clause la liste de ses variables vivantes mais avec watch literals ça tue l'idée de la méthode,
on ne veut rien faire sur litéraux non watched, cela se répercute au niveau des bets qui sont donc plus lent car il faut tester tous les litéraux tout cela est facilement implémentable avec notre archi.    

Tous ces détails d'implémentations relatifs à chaque méthode sont donc écrits dans SATSolverSTD (standard) et SATSolverWL, sachant qu'ils ont toujours accès aux fonctions qu'ils partagent implémentées dans SATSolver.    

Cela permet par exemple (pour l'instant) d'avoir une fonction solve() ou deduce() générique.   
Les fonctions virtuelles pures sont celles qui représentent les changement à implémenter dans chaque variante de DPLL.  
Idem pour les bets et les deux parsers.  

### Améliorations:         

Les heuristiques sont lentes avec les WL on doit creuser pour comprendre pourquoi.     


## Générateur de formules CNF
Afin de générer une formule CNF, il suffit d'éxécuter le programme sans aucuns arguments. Un menu vous sera alors présenté et vous aurez le choix de générer une formule SAT CNF. Pour ce faire suivez les instructions.       

## Structure des fichiers


## Performances
###Optimisation à la compilationTous les tests
présentés ont été fait en -O3.  
Un exemple de comparaison sur aim-100-1_6-no-1.cnf :  

Sans flag -O2 : 37.75s

Avec flag -O2 : 7.052s

### Dummy tests CNF

Tous les tests dummy_ dans dev/bin/test_base/cnf montrent les fonctionnalités implémentées
dans DPLL, pour bien le voir lancer en -debug.

* \_sat: dummy satisfiable
* \_taut: dummy tautologie
* \_uniquepol: dummy deduce avec 1 seule polarité
* \_unitprop: dummy deduce clauses unitaires
* \_unsat: dummy unsat

### Base de test CNF (cf: dev/bin/test_base/cnf)

#### simple_v3_c2.cnf
SATIFIABLE

Temps d'éxécution : 0.003s

#### quinn.cnf
SATIFIABLE

Temps d'éxécution : 0.002s

#### aim-50-1_6-yes1-4.cnf
SATIFIABLE

Temps d'éxécution : 0.006s

#### aim-100-1_6-no-1.cnf
UNSATIFIABLE

Temps d'éxécution : 7.052s

#### hole6.cnf
UNSATISFIABLE

Temps d'éxécution : 0.761s

#### dubois20.cnf
UNSATISFIABLE

Temps d'éxécution : 54.320s

#### dubois21.cnf
UNSATISFIABLE

Temps d'éxécution : 1m49.677s

#### dubois22.cnf
UNSATISFIABLE

Temps d'éxécution : 3m50.931s

#### par8-1-c.cnf
SATIFIABLE

Temps d'éxécution : 0.028s

### Dummy tests FOR

* \_ant: dummy antithèse
* \_con: dummy conjonction
* \_dis: dummy disjonction
* \_eq: dummy equivalence
* \_imp: dummy implication
* \_neg: dummy negation
* \_xor: dummy xor
* \_simple: dummy simple

### Base de test FOR (cf: dev/bin/test_base/for)
* \_test
* \_test1
* \_test2
* \_test3

### La clause de l'horreur

dev/bin/test_base/cnf/bf0432-007.cnf

On ne finit pas en un temps raisonnable (> 1jour aux derniers tests).  
Minisat la finit en 1s.  

## Qui a fait quoi
#Tristan
Implémentation DPLL, prétraitement, optimisation avec liste de priorité, structures de données et tests, Watched Literals, redesign en statégies, scripts de tests.
#Alexy
Parser CNF, prétraitement, transformation de Tseitin, structures de données, interface, générateur de formules et tests, heuristiques de paris, redesign en statégies, scripts de tests.
