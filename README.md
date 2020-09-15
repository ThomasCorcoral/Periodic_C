## Quel est ce projet ?
Le but de ce projet est de faire dialoguer deux programmes entre eux. Au final on aura un programme qui écoute en continue et un second avec qui il communiquera. Ce second sera appelé dans un autre terminal. Les deux programmes communiqueront grace au PID du premier stocké dans un fichier.<br><br>
Le second permettra d'envoyer au premier une date, une fréquence et une commande. Cette commande sera alors executé à partir de la date renseigné et ensuite répété à un intervalle régulier défini par la fréquence. Tous les arguments sont en secondes.<br>
<i>Pour plus de détails le fichier Details.txt répond aux questions du projet et donc explique bon nombre de choix techniques</i>

## Demo

![Preview](https://github.com/ThomasCorcoral/Periodic_C/blob/master/periodic.png)

## Comment a-t-il été réalisé ?

Le projet a entièrement été réalisé en C#.

## Guide d'installation

#### Etape 1
Clonez le projet Github. Rendez vous dans le dossier.

#### Etape 2
Créez les executables en tapant make

#### Etape 3
Ouvrez <b>2</b> terminaux

#### Etape 4 
Dans le premier terminal lancez period (./period)

#### Etape 5
Dans le second terminal lancez les commandes souhaitées<br>
par exemple ./periodic 0 5 ls<br>

#### Etape 6
Pour afficher à nouveau le viewer, il vous faudra le relancer dans le premier terminal (./race-viewer)

#### Etape 7
Pour voir les commandes rentrées ./periodic dans le second termial<br>
Pour arrêter period ctrl+c dans le premier.
