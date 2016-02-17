# SATSolver
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216
Powered by git: https://github.com/Oxmose/SATSolver

## Arguments de la ligne de commande
./reso1 : Démmare l'interface utilisateur du programme.

Arguments
-tseitin <file_name> parse et résout le fichier file_name à l'aide de la transformation de Tseitin.

<file_name> parse et résout le fichier file_name à la norme DIMACS CNF.

Vous pourrez utiliser -debug en dernier argument afin d'activer les sorties de debug du programme.
Ex : ./reso1 -tseitin test.for -debug

## Choix d'implémentation DPLL

Le coeur de DPLL est SATSolver::solve() (Core/SATSolver.cpp).
On y prend des décisions qui sont soit des bets soit des déductions.
Les décisions sont stackées dans m_currentAssignement.

La formule que l'on manipule est décomposée en deux ensembles de clauses :
    -les non actuellement satisfaites (m_formula[0])
    -les actuellement satisfaites (m_formula[1])

Ces deux ensembles de clauses sont munis d'une structure de multiset dont l'ordre
est différent.
On considère les clauses non satisfaites dans l'ordre de leur ID.
On trouve les clauses non satisfaites qui possèdent la variable d'index x en
nlog(n) grâce à la map précalculée m_clauseWithVar qui associe à chaque variable
la liste des clauses qui la contienne et le multiset.

On considère les clauses satisfaites par classe d'équivalence de même "satisfier" :
quand on satisfait une clause c'est grace à un satisfier (index de la variable qui a permis de satisfaire)
et donc pour les ressusciter dans le backtrack on appelle toutes les clauses avec le satisfier i.
C'est ici qu'est vraiment exploitée la structure de multiset.
On a choisi de la garder pour les deux par soucis d'uniformité.

Les clauses contiennent sont construites à l'image de la formule.
Deux sets : les littéraux assignés et ceux non assignés.
Cela permet de détecter efficacement le cas UnitProp et de manière générale de travailler
avec uniquement ce dont on a besoin.
L'impossibilité d'avoir deux index (set vs multiset) n'est pas génante : on enlève les doublons (1\/1) au parsing
et les tautologies qui contiendraient i et -i sont flushée au début et jamais considerée dans l'algo.

Le prétraitement n'est qu'un cas particulier qui est traité dans la boucle principale.
En effet, tant qu'on peut déduire initialement takeABet() qui prend les paris n'est pas
appelée.

takeABet() met à vrai la première variable vue non assignée.
On constate des différences de performances en moins ou en plus suivant les tests
avec les copains qui prennent les paris dans l'ordre croissant pour l'index de la variable.

Le nom des fonctions et la construction de la boucle ont été pensés pour tracer au mieux
le déroulé de DPLL.

On retient la valuation en cours dans la map m_valuation et on met arbitrairement la valeur 1
aux variables non assignées à la fin de l'algo (m_valuation[index] = -1). (SATSolver::showSolution() l.321)
Cela peut être vu comme redondant d'avec m_currentAssignement mais le role de ces deux structures n'est pas du tout
le même dans l'implémentation : l'un central dans le backtrack et l'autre purement visuel pour afficher la solution.

Ces choix d'implémentation se révèlent payant en pratique: on gagne en moyenne 30% de temps de calcul par rapport
à l'implémentation naïve (l'un des premiers commits sur le git).

Ce qui n'est pas méga satisfaisant sur le principe:
    -les copies que l'on doit faire pour passer de m_formula[0] à m_formula[1]
    et vice versa (delete/insert), voir SATSolver::satisfyClause.
    -idem avec les littéraux (Clause::setAssigned)
    -surement d'autres choses (m_formula[0] multiset au lieu de set, peut on faire mieux ?)

## Parsers
Le parser CNF permet de parser les fichiers CNF au format DIMACS.

Le parser LOG permet de parser les fichiers FOR qui contiennent des clauses logiques plus conviviales pour l'utilisateur.

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
