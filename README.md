# SATSolver
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216

## Parsers
-Le parser CNF permet de parser les fichiers CNF au format DIMACS.

-Le parser LOG permet de parser les fichiers FOR qui contiennent des clauses logiques plus convivialles pour l'utilisateur.

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

Sans flag -O2 : 37,75s

Avec flag -O2 : 7,02s
