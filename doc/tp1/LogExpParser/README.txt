
1/ Les conflits shift/reduce
  --------------------------

pour compiler, lancer
make

pour executer le programme, lancer
./calc

entrer une expression arithmetique (avec juste + et *)
valider l'expression avec Ctrl-D (end of file)


lors de la compilation de l'analyseur syntaxique (bison), 
allez contempler le fichier "expr.output"

Exemple: dans expr.ypp, supprimez la ligne commencant par "%left
TK_MULT", sauvegardez, et recompilez. En principe, vous voyez apparaitre
des messages "shift/reduce conflicts" lors de la compilation (qui
aboutit neanmoins: une ambiguite est signalee, mais une decision par
defaut est prise).

Ouvrez a nouveau le fichier expr.output

Vous voyez que le fichier commence par lister les conflits puis associer 
des nombres aux regles de grammaire:

<<
    0 $accept: input $end

    1 input: expression

    2 expression: TK_INT
    3           | TK_LPAREN expression TK_RPAREN
    4           | expression TK_PLUS expression
    5           | expression TK_MULT expression
>>

en cherchant "Conflit" dans le fichier, vous tombez sur:

<<
État 10

    4 expression: expression . TK_PLUS expression
    4           | expression TK_PLUS expression .  [$end, TK_PLUS, TK_MULT, TK_RPAREN]
    5           | expression . TK_MULT expression

    TK_MULT  décalage et aller à l'état 8

    TK_MULT   [réduction par utilisation de la règle 4 (expression)]
    $défaut  réduction par utilisation de la règle 4 (expression)

    Conflit entre la règle 4 et le jeton TK_PLUS résolu par réduction (%left TK_PLUS).
>>

L'etat 10 de l'automate signale une ambiguite: il a le choix entre
d'une part faire "shift" lorsqu'il lit TK_MULT, puis passer a l'etat 8,
et d'autre part faire "reduce" pour reconnaitre qu'il peut appliquer
la regle 4. Par defaut, on lit qu'il fait une réduction, ce qui le conduit a
l'etat 8, ou l'on peut voir que le TK_MULT a ete empile (le point s'est
"deplace sur la droite"):

État 8
    5           | expression TK_MULT . expression


2/ Extension de la grammaire: le symbole moins
  --------------------------------------------

En l'etat, on ne peut reconnaitre que des sommes ou multiplications
d'entiers positifs. 

Etendez le programme (en modifiant les differents fichiers) de maniere
a pouvoir traiter
 . les soustractions, et
 . le "moins unaire"

Il faudra pouvoir taper des expressions comme
  3 + 5 - (-4)
  3 + - (10+2)

Indication: le moins unaire doit avoir la priorite la plus grande, et
ce n'est pas un operateur binaire comme +, - ou *. 

