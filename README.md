# TSAT
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216
TSAT
Powered by git: https://github.com/Oxmose/SATSolver

## Arguments de la ligne de commande
./resol : Démmare l'interface utilisateur du programme où tout est expliqué.

## Résumé rendu 4
Pour le rendu 4, l'algortithme DPLL à été totallement recodé afin de corriger certain bugs réccurents et d'alléger le code. Le solveur gère maintenant DPLL standard, avec litéraux surveillés et avec apprentissage de clauses.
Nous avons implémenté les solveur SMT pour l'égalité et la congruence. Seul le parsing des formules SMT pour la logique de différence à été ralisé mais n'est interfacé à aucun solveur.
Le clause learning est actif pour SMT pour l'égalité.
L'ancien noyau du solveur est sauvegardé dans le dossier "Old" dans le dossier des sources. Le nouveau noyau est venu le remplacer. Le noyau SMT quand à lui se trouve dans le dossier "Core/SMT".

### Améliorations:
- Refonte totale de DPLL, allègement du code et des traitements.
- Alègement des heuristiques de pari.
- Possibilité de désactivation de CL avec SMT pour l'égalité : -disable_smt_cl

## Performances
Un set de test à été réalisé pour récupérer quelques résultats de temps de notre parser. Pour consulter le graphique des temps moyens, ouvrez le fichier time.png dans le dossier "doc" du projet.

## Qui a fait quoi
###Tristan
* rendu1 : Implémentation DPLL, prétraitement, optimisation avec liste de priorité, structures de données et tests.
* rendu2 : Refonte DPLL, implémentation Watched Literals, redesign en statégies, scripts de tests, optimisation.
* rendu3 : Implémentation CL, CL+WL, recherche de l'UIP dans le graph linéaire, scripts de test, corrections de bugs, optimisation.
* rendu4 : Réimplémentation de DPLL, WL et CL, implémentation SMTE/SMTC, rapport.

###Alexy
* rendu1 : Parser CNF, prétraitement, transformation de Tseitin, structures de données, interface, générateur de formules et tests.
* rendu2 : Heuristiques de paris, redesign en statégies, modifications du parser, scripts de tests, optimisation.
* rendu3 : Recherche de l'UIP dans le graph (quadratique et linéaire), VSIDS, interface, scripts de test, corrections de bugs, optimisation.
* rendu4 : Structures de données (UnionFind,...), parser et prétraitement pour SMTE, SMTC, SMTD, rapport.

## Retour sur les algorithmes de détection de UIP (Nouveau, rendu 4)
### Linéaire
Nous avons implémenté un algorithme linéaire trouvé pour l'occasion. Étant donné un DAG à une source et un puit on souhaite trouver les noeuds tels que tous les chemins de la source au puit passe par eux : les UIP. On cherche plus particulièrement celui qui est le plus proche du puit.
On s'appuie sur la remarque suivante : tous les UIP sont sur le plus court chemin entre la source et le puit.
Nous sommes dans un cas particulier où le puit a exactement deux pères. Pour ces deux noeuds on calcule le plus haut noeud du plus court chemin auquel il peut remonter sans passer par les arêtes du plus court chemin. Le plus haut des deux est le 1-UIP. Cet algorithme est linéaire.
