# SATSolver
SAT Solver by Tristan Stérin and Alexy Torres at the ENS Lyon 2015-216
TSAT
Powered by git: https://github.com/Oxmose/SATSolver

## Arguments de la ligne de commande
./reso1 : Démmare l'interface utilisateur du programme tout est expliqué.

## Résumé rendu 3

On travaille uniquement avec le graphe que l'on maintient tout le temps à jour. On trouve le 1-UIP avec un algo linéaire similaire à articulation point. Nous avons rencontrés de gros soucis avec la gestion de l'apprentissage des clauses unitaires. Notre implémentation de CL est assez rustinée mais semble correcte. Nous avons cependant des exemples non corrects pour CL + VSIDS (test_base/cnf/corr_test/sat/aim-100-3_4-yes1-1.cnf) et CL WL (test_base/cnf/corr_test/sat/aim-100-1_6-yes1-3.cnf).    
Nous n'avons pas implémenté -forget, avec uniquement le graphe comme outil -forget est assez couteux et aussi nécessiterait que l'on change pas mal de choses dans l'architecture, d'autres binômes nous ont dits que les gains étaient de plus faibles. Nous l'implémenterons au rendu 4 si nécessaire.  
Nos graphes sortent en .dot avec l'option -cl-interact, le nom du fichier est également outputé. La commande "dot -Tpng conflictGraphx.dot -o conflictGraphx.png" permet la conversion par exemple. Nous avons pris quelques libertés sur la légende car cela nous a beaucoup servi pour débugguer, avec du rouge pour le conflit, du vert pour le bet de niveau courant et du rose pour les noeuds de la clause apprise hors UIP.   

### Améliorations:         

- Notre architecture est clairement caduque dans le cadre du CL. Tout devient compliqué et se bug très facilement, il faut que l'on reprenne ça.   
- CL-WL nous semble vraiment vraiment faux tels que conçu dans notre programme il faut que l'on y re-réfléchisse. Notamment quoi faire quand on ne watch pas les bons littéraux dans une clause apprise..


## Structure des fichiers


## Performances
On obtient des "s UNSATISFIABLE" bien plus rapidement qu'avant avec -cl -vsids. Les dubois2x prenaient entre 1 et 2 minutes au dernier rendu contre quelques millisecondes maintenant. Aussi la clause que l'on arrivait pas à faire finir ("bf0432-007.cnf") termine maintenant en 20 secondes.


## Qui a fait quoi
#Tristan
* rendu1 : Implémentation DPLL, prétraitement, optimisation avec liste de priorité, structures de données et tests.   
* rendu2 : Refonte DPLL, implémentation Watched Literals, redesign en statégies, scripts de tests, optimisation.    
* rendu3 : Implémentation CL, CL+WL.

#Alexy
* rendu1 : Parser CNF, prétraitement, transformation de Tseitin, structures de données, interface, générateur de formules et tests.
* rendu2 : Heuristiques de paris, redesign en statégies, modifications du parser, scripts de tests, optimisation.
* rendu3 : Recherche de l'UIP dans le graph (quadratique et linéaire), VSIDS, interface, scripts de test, corrections de bugs, optimisation.

## Retour sur les algorithmes de détection de UIP
### Naif
Le premier algorithme implémenté pour la détection du premier UIP se déroule en temps quadratique. Pour chaque noeuds dans le graph, on retire le noeud. On parcour ensuite le graph du noeud de conflit. Si l'on atteind la racine alors ce noeud n'est pas un UIP. Une fois tous les UIP détectés, on choisi le premier en faisant un BFS à partir du conflit et pour chaque noeuds rencontrés, on vérifie s'il est dans la liste des UIP. Au premier UIP rencontré on le retourne.
### Linéraire
Le second algorithme se déroule en temps linéaire en le nombre de noeuds et d'arrêtes du graphe.
* Dans un premier temps, on produit le graphe inverse (G-1).
* Puis parcours BFS à partir du dernier BET, on recréé alors un graph de ce BET vers le noeud de conflit en syétrisant les arrêtes.
* On applique un algorithme de détéction de points d'articulations dans ce nouveau graph.
* On parcours le graph G-1 à partir du noeuds de conflict, le premier noeud rencontré étant un noeud d'articulation dans le graph symétrisé est le First UIP.
