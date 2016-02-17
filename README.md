# SATSolver
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216

## Arguments de la ligne de commande
./reso1 : Démmare l'interface utilisateur du programme.

Arguments
-tseitin <file_name> parse et résoud le fichier file_name à l'aide de la transformation de Tseitin.

<file_name> parse et résoud le fichier file_name à la norme DIMACS CNF.

Voud pourrez utiliser -debug en dernier argument afin d'activer les sorties de debug du programme.
Ex : ./reso1 -tseitin test.for -debug

## Parsers
Le parser CNF permet de parser les fichiers CNF au format DIMACS.

Le parser LOG permet de parser les fichiers FOR qui contiennent des clauses logiques plus convivialles pour l'utilisateur.

Améliorations:


Le parser LOG applique la transformation de Tseitin à la formule originelle et parse de nouveau la formule récupérée afin de la rendre compatible avec nos structures de données, il serait plus convenable d'intégrer cette transformation directement dans le parser.

## Générateur de formules CNF
Afin de générer une formule CNF, il suffit d'éxécuter le programme sans aucuns arguments. Un menu vous sera alors présenté et vous aurez le choix de générer une formule SAT CNF. Pour ce faire suivez les instructions.

## Structure des fichiers
-doc contient le premier TD

-dev/bin contient l'éxécutable de notre programme

-dev/bin/test_base/cnf contient les formules CNF utilisées pour les tests

-dev/bin/test_base/log contient les formules logiques utilisées pour les tests

-dev/src contient les sources du programme (main et makefile à la racine)

-dev/src/CNFParser contient les sources du parser CNF

-dev/src/LogExpParser contient les sources du parser de formules logiques

-dev/src/Core contient les sources des structures de données et du coeur du solveur SAT

-dev/src/Global contient les variables globales et toutes autre macros relative au debug de l'application

-dev/src/RandomSatExpGenerator contient les sources sur générateur d'expressions CNF.

## Performances
###Optimisation à la compilation

Test de aim-100-1_6-no-1.cnf 

Sans flag -O2 : 37.75s

Avec flag -O2 : 7.052s

### simple_v3_c2.cnf 
SATIFIABLE

Temps d'éxécution : 0.003s

### quinn.cnf
SATIFIABLE

Temps d'éxécution : 0.002s

### aim-50-1_6-yes1-4.cnf 
SATIFIABLE

Temps d'éxécution : 0.006s

### aim-100-1_6-no-1.cnf 
UNSATIFIABLE

Temps d'éxécution : 7.052s

### hole6.cnf 
UNSATISFIABLE

Temps d'éxécution : 0.761s

### dubois20.cnf 
UNSATISFIABLE

Temps d'éxécution : 54.320s

### dubois21.cnf 
UNSATISFIABLE

Temps d'éxécution : 1m49.677s

### dubois22.cnf 
UNSATISFIABLE

Temps d'éxécution : 3m50.931s

### par8-1-c.cnf
SATIFIABLE

Temps d'éxécution : 0.028s
