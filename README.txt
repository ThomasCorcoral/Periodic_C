Corcoral Thomas
Pape Lucas
Groupe TP1B


Exercice 1 :

utilisation des fonction time(2) et ctime(3).

Exercice 2 :

1 - On calcule la taille de la chaine de caractère passé en paramètre. Puis on envoie l'adresse de cet entier.
    On envoie ensuite la chaine de caractères dans le pipe. On free la chaine. Ensuite on s'occupe de la chaine 
    de caractère à envoyer. On l'envoie dans le pipe puis on vérifie que tout s'est bien passé. On renvoie alors 
    la taille de la chaine ou bien -1 si erreur.

2 - On récupère la taille de la chaine qui va arrivé en première.
    Puis on récupère la fonction dans une nouvelle chaine de caractère allouée grace à la taille récupéré précedemment.

3 - Pour pouvoir envoyer un tableau de chaine de caractère, il faut tout d'abord envoyer ça taille, puis à l'aide de
    la fonction rédigée précedemment on envoie au toutes les chaines précédées de leurs longueurs.

4 - Cette fonction va récupérer les différentes informations arrivant par le pipe

Exercice 3 :

Pour cet exercice, j'ai décidé de mettre en place une structure pour réalisé une chaine de caractère allouée dynamiquement.
Pour ce faire j'ai également rajouté deux fonctions, str_grow(struct chaine *self); et str_free(struct chaine *self); qui permettent
d'agrandir une chaine de caractère et de libérer l'espace qu'une chaine occupe.
Cette structure va être utile pour récupérer le PID de périodic (s'il existe) car on ne connait pas à l'avance la taille du fichier à lire.

Exercice 4 :

1 - Pour cette partie, je vérifie que le nombre d'argument fournient au programme est valide à savoir plus de trois ou aucun puis en fonction 
    de ce nombre d'argument, on traite les arguments fournient afin de savoir s'ils sont valides.

2 - Envoie du signal SIGUSR1 via un kill à period en récupérant son PID qui a été préalablement inscrit dans le fichier /tmp/period.pid

3 - Envoie des informations dans le tube. On vérifie tout d'abord que le fichier existe puis que l'ouverture du tube s'est produite sans erreur
    Pour pouvoir envoyer toutes les informations, nous avons eu besoin de créer une fonction qui permet d'envoyer un entier dans le tube et également
    de le récupérer pour pouovoir l'utiliser dans period

Exercice 5 :

1 - Envoie du signal SIGUSR2 via un kill à period en récupérant son PID qui a été préalablement inscrit dans le fichier /tmp/period.pid
    Mise en place d'un booléen qui détermine le signal envoyé entre SIGUSR1 et SIGUSR2

2 - La reception est mise en place grace à la selection faites précedemment.

Exercice 6 : 

1 - Avant d'écrire ou bien même d'ouvrire le fichier, on vérifie qu'il exisiste. Si c'est le cas, on réalise un exit car cela signifie que period est
    déjà lancé. En effet lors de la fin du programme on supprime ce fichier. S'il n'existe pas, on peut lancer le programme et donc écrire le pid dans
    le fichier.

2 - Cette partie est effectuée par la procedure void redirect_out(); qui va rediriger stdout et stderr vers des fichier à l'aide de dup2()

3 - On vérifie l'existance du tube à l'aide de la fonction stat() puis si le tube n'existe pas, on le crée à l'aide de mkfifo()

4 - On vérifie l'existance du dossier avec la fonction stat(), ensuite si le dossier n'existe pas, on le crée à l'aide de mkdir()

Exercice 7 : 

1 - Pour chaque commande il existe une structure qui comporte la date de début, la fréquence d'execution, la taille de la commande (commande + nombre d'arguments)
    et également les chaine(s) de caractères

2 - Pour la structure liste, la taille de la liste, la capacité de cette dernière (pour l'allocation dynamique) et les différentes commandes

Exercice 8 :

Pour la gestion du signal SIGUSR1, on va utiliser un "volatile sig_atomic_t flag;" afin de pouvoir y accéder depuis la fonction handler et également
depuis le main pour pouvoir executer les bonnes fonctions

Exercice 9 : 

Même principe que pour la 8 sauf que ici, au lieu de recevoir et de traiter, on va juste envoyer toutes les informations contenues dans la liste

Exercice 10 : 

1 - On parcours la liste pour savoir combien de temps le programme va rentrer en pause, et on cherche le plus petit délai.
    Ensuite, si nous avons trouvé une commande, on définit l'alarm en fonction du délai défini.

2 - On met en place l'écoute sur SIGALRM. On attend un signal et lorsque le signal correspond, on fork, et on redirige stdin, stdout et stderr
    vers les chemins adéquat. Enfin on execute la commande tout en vérifiant sa fréquence. Si la fréquence est nulle, on supprime de la liste.

Exercice 11 :

On gère le signal SIGCHLD avec waitpid et l'argument WNOHANG, qui lui va nous permettre de revenir immédiatement si aucun fils n'est achevé.
Lorsque le pid est différent de 0, alors on affiche l'état du fils. Sinon on sort avec break.

Exercice 12 :

Lorsqu'on recoit le signal SIGINT, ou SIGTERM ou SIGQUIT, on supprime periode.pid avec remove (si la fonction renvoie un nombre différent de 0).
Ensuite, on libère l'espace mémoire alloué dynamiquement avec la fonction free_liste et enfin, on termine et élimine tous les processus créés restant avec kill.

Exercice 14 :

On a divisé launch_daemon en plusieurs fonctions. La première est celle de l'initialisation, dans laquelle on change le répertoire courant à / via chdir(2). 
On change aussi le umask à 0 via umask(2), et enfin on fermer tous les descripteurs de fichiers standard avec close(0), close(1) et close(2).
Une autre fonction finish() qui elle sert juste a fermer le fichier "flog" de type FILE*, qui nous sert pour la question bonus.
La dernière fonction, nommée process, vérifie le nombre d'arguments et execute la ou les commandes.
Toutes ces fonctions sont réunies dans le main, dans lequel on utilise un double fork. Lorsqu'on est dans le parent, on attend, sinon dans le fils, le fils devient leader de session via setsid(2).
Puis, on fait le deuxieme fork. Si c'est différent de 0 alors on sort avec success. Sinon, on appelle ensuite la fonction process() et pour terminer la fonction finish().

Exercice 15 :

Pour cet exercice bonus, on a tout d'abord modifier la fonction handler et rajouter la fonction get_sigmatch dans period.c. Pour mieux gérer les signaux dans le main, le binaire était la solution.
Donc on à remplacé les flags par l'appel de la fonction get_sigmatch. 
On à ajouter le caractere spécial "++DELETE++" dans message.c que l'on récupère dans period.c pour supprimer la commande adéquat de la liste.
Dans periodic.c, on ajouter la condition lorsque que l'utilisateur tape "delete". Lorsque c'est le cas on récupere le numéro de la commande que l'on souhaite supprimée pour arreter le processus de cette commande
et la retirer de la liste des commandes.

Conclusion / Bilan :

Nous avons réussi à faire toutes les questions demandées, y compris la question bonus.
Ce résultat à été rendu possible grace à notre bonne gestion de groupe. 
Nous avons partagé comme il faut le travail, tout en s'aidant mutuellement.
