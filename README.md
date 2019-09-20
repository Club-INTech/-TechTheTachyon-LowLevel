# TechTheTachyon-LowLevel
## SETUP
NB: Plusieurs aspects de configuration du projet sont pensés pour utiliser platformio avec CLion et ne sont ni testés ni prévus pour d'autres IDE.

Pour pouvoir utiliser le projet:
- Installer Platformio ***version > 4.0*** ([Guide d'installation](https://club-intech.minet.net/images/9/97/Guide_PIO.pdf))
- Clonez ce dépot
- Deux possibilités : 

1. 
   - **Initialisez le sous-module DynamixelCom avec la commande** `git submodule update --init lib/Dynamixel-Com`
   - Ouvrez un terminal dans le dossier du dépot, et effectuez la commande \
     ```pio init --ide IDE --board teensy35```\
     avec IDE=clion si vous utilisez CLion, ou vscode si vous utilisez Visual Studio Code
2. 
   - ```./setup.sh``` Uniquement pour CLion pour le moment. (Peut se charger de l'installation de platformio avec apt et pacman)
- Ouvrez le dossier du dépot avec CLion ou VSCode.
- Gagnez la coupe

---

Pour CLion, si vous voulez profiter de la gestion automatique des `#if defined()` :
 - Settigns -> Build, Execution, Deployment -> CMake
 - En dessous de "Profiles" il y a une liste avec seulement `Debug` dedans. Appuyez deux fois sur le plus.
 - Vous devez maintenant avoir une liste avec `Debug`, `Main` et `Slave`. Vous pouvez supprimer `Debug` si vous le voulez.
 - Dans l'éditeur, là où vous choisissez de de build ou d'upload, vous pouvez maintenant sélectionner `Main` ou `Slave` !
 
Vous aurez à répéter l'ajout des profils à chaque fois que vous ferez un `pio init [...]`.
 
## UTILISATION

Afin de fusionner les branches des deux robots et d'éviter les divergences, un système à base de define et de `#if defined` \
a été mit en place. Cela change légèrement la façon dont est géré le build et l'upload :

 - ***Ne pas utiliser `PLATFORMIO_BUILD` ou `PLATFORMIO_UPLOAD`***.
   - L'utilisation d'un environnement platformio "leurre" pour éviter la détection de define automatique de platformio implique que le leurre ne soit pas compilable. Ces commandes tenteront de le compiler/uploader et échoueront car de nombreuses variables et fonctions ne seront pas définies.
 - Utiliser les cibles `INTECH_[X]_[ROBOT]` en fonction de si vous voulez build/upload pour le principal (MAIN) ou le secondaire (SLAVE)
   - La cible `INTECH_BUILD_BOTH` permet de compiler pour les deux robots en même temps.
   
Vous n'avez pas besoin d'avoir ajouté les profils CMake pour pouvoir effectuer ces actions.
 
## TODO

- [x] Compléter le nouveau MCS
- [x] Asservissement
- [x] Vérifier l'état de la communication LL/HL
- [x] Implémenter les nouveaux ordres
- [ ] Ordre propre pour la récupération de données d'asservissement + scripts en conséquence
- [ ] Mise en place de cas d'erreur explicites et plus nombreux pour le HL
- [ ] Gagner la Coupe

## TABLE DES ORDRES
### ORDRES HL ⇒ LL
#### ORDRES GENERAUX

|        Ordres         |                           Actions                         |
|:---------------------:|:---------------------------------------------------------:|                  
|         ping          |                         Ping le LL                        |
|          j            |           Active l'attente de l'activation du jumper      |
|         sus           |        Switch les US ou choisit leur état (on/off)        |
|          f            |                     Check le mouvement                    |
|        ?xyo           |                   Position + Orientation                  |
|          d            |                      Translate de x mm                    |
|          t            |     Tourne de α rad, dans le sens demandé ou librement    |
|        stop           |                            Stop                           |
|         cx            |                       Set x d'origine                     |
|         cy            |                       Set y d'origine                     |
|         co            |                       Set α d'origine                     |
|        cxyo           |                     Set x,y,α d'origine                   |
|        ctv            |                    Set translation speed                  |
|        crv            |                     Set rotation speed                    |
|        ctrv           |               Set translation+rotation speed              |
|        efm            |                    Enable forced movment                  |
|        dfm            |                   Disable forced movment                  |
|        ct0            |              Désactive l'asserv en translation            |
|        ct1            |               Active l'asserv en translation              |
|        cr0            |               Désactive l'asserv en rotation              |
|        cr1            |                 Active l'asserv en rotation               |
|        cv0            |                Désactive l'asserv en vitesse              |
|        cv1            |                 Active l'asserv en vitesse                |
|        cod            |                Retourne les ticks de codeuse              |
|      pfdebug          |                Info de debug sur la position              |
|      rawpwm           |            Demande un PWM brut aux deux moteurs           |
|      getpwn           |              Retourne le PWM des deux moteurs             |
|      errors           |             Retourne les erreurs d'incertitude            |
|      rawspeed         |                   Vitesse brute des roues                 |
|     rawposdata        |             Pos x,y,α; vL,vR, targetvL,targetvR           |
|     montlhery         |                    Mode de présentation                   |
|         av            |                           Avance                          |
|         rc            |                           Recule                          |
|         td            |                       Tourne à droite                     |
|         tg            |                       Tourne à gauche                     |
|       sstop           |                            Arrêt                          |
|       maxtr           |                Vitesse maximale de translation            |
|       maxro           |                  Vitesse maximale de rotation             |
|         nh            |     Créé un nouveau hook (id,x,y,r,α,tolerance,action)    |
|         eh            |                       Active le hook                      |
|         dh            |                     Désactive le hook                     |

### Ordres pour les capteurs
|   Ordres  |                       Actions                                          			| Arguments      				|
|:---------:|:---------------------------------------------------------------------------------:|:------------------------------|
|lectureSICK|Renvoie les 6 distances lues par les SICK (dans le sens trigo)          			|N/A             				|
|testSICK	|Renvoie la valeur d'un seul capteur SICK								 			|indice du capteur             	|
|rangeSICK	|Configure la fenêtre de détection d'un SICK (pour que le LL connaisse les valeurs)	|indice, min, max	            |


#### ORDRES DE CONTRÔLE D'ACTION

|   Ordres      |                       Actions                      | Arguments                 |
|:-------------:|:--------------------------------------------------:|:--------------------------|
|    XLm        |Envoie le XL-430 à un α en °                        |id XL / α                  |
|    XLs        |Modifie la vitesse d'un XL-430                      |id XL / speed              |
|    posBras    |Récupère les angles (en °) d'un bras                |side(left/right)			     |
|	brasToutDroit	|Envoie le bras à la position "tout droit"           |side(left/right)           |
|    dist   		|Envoie le bras à la position "distributeur"         |side(left/right)           |
|   grnd    		|Envoie le bras à la position "sol"                  |side(left/right)           |
|   stock   		|Envoie le bras à la position "ascenceur"            |side(left/right)           |
|    acc    		|Envoie le bras à la position "accélérateur"         |side(left/right)           |
|    posinter   |Envoie le bras à la position "intermediaire"        |side(left/right)           |
|     up    		|Monte l'ascenceur de la hauteur d'un palet          |side(left/right)           |
|    down   		|Descend l'ascenseur de la hauteur d'un palet        |side(left/right)           |
|    suck   		|Active la pompe                                     |side(left/right)           |
|  unsuck   		|Désactive la pompe                                  |side(left/right)           |
|    valve   		|Active ou désactive l'électrovanne                  |side(right/other)/(on/other)|
|  valveon  		|Active l'électrovanne                               |side(left/right)           |
|  valveoff 		|Désactive l'électrovanne                            |side(left/right)           |
|   gold        |Envoie le bras à la position "goldonium"            |      /                    |
|   bal         |Envoie le bras à la position "balance"              |side(left/right)           |
|   elec        |Démarre l'électron                                  |      /                    |
| torqueBras    |Donne la couleur du palet pris (selon le couple)    |side(left/right) / position|
|  torqueXL     |Donne le couple d'un XL                             |id XL                      |




### ORDRES SPECIFIQUES LL

|   Ordres          |                       Actions                      |
|:-----------------:|:--------------------------------------------------:|
|   reseteth        |            Redémarre le module ethernet            |
|   toggle          |         Change le mode de réglage d'asserv         |
|   displayAsserv   |          Retourne les constantes d'asserv          |
|   kpt             |              Set le kp de translation              |
|   kdt             |              Set le kd de translation              |
|   kit             |              Set le ki de translation              |
|   kpr             |                Set le kp de rotation               |
|   kdt             |                Set le kd de rotation               |
|   kit             |                Set le ki de rotation               |
|   kpg             |            Set le kp de vitesse à gauche           |
|   kdg             |            Set le kd de vitesse à gauche           |
|   kig             |            Set le ki de vitesse à gauche           |
|   kpd             |            Set le kd de vitesse à droite           |
|   kdg             |            Set le kd de vitesse à droite           |
|   kig             |            Set le ki de vitesse à droite           |
