# Jeu du Labyrinthe

## Présentation du projet

Ce projet consiste en la conception et la réalisation complète en **langage C (norme C99)** d'un générateur de labyrinthes et d'un jeu d'exploration en mode console.  

Il valide l'ensemble des 4 étapes décrites dans le cahier des charges :
1. **Étape 1** : Générateur de labyrinthe parfait (taille fixe 11×25) via l'algorithme de fusion aléatoire de chemins (Kruskal) et affichage en mode console.
2. **Étape 2** : Menu interactif, choix des dimensions impaires, sauvegarde/chargement au format `.cfg` et déplacements au clavier.
3. **Étape 3** : Objets interactifs (clé déverrouillant la porte, bonus de points, malus de pièges), calcul du score en fonction des pas et classement persistant du Top 10 (`.score`).
4. **Étape 4** : Deux niveaux de difficulté (Facile/Difficile), génération de cycles (chemins multiples) et monstres autonomes (Fantômes et Ogres) régis par des **pointeurs vers des fonctions**.

---

## Manuel du Joueur

### Objectif
Vous incarnez le joueur `o` débutant son périple en haut à gauche de la grille `(1, 0)`. Votre but est de vous frayer un chemin à travers les couloirs pour atteindre la sortie `-` située en bas à droite `(largeur - 2, hauteur - 1)`.

La porte de sortie est verrouillée à double tour : **vous devez obligatoirement trouver et ramasser la clé (`K`) avant de pouvoir vous échapper.**

### Commandes et contrôles
* **Déplacements** :
  * **Haut** : Touche `Z` ou `Flèche Haut`
  * **Bas** : Touche `S` ou `Flèche Bas`
  * **Gauche** : Touche `Q` ou `Flèche Gauche`
  * **Droite** : Touche `D` ou `Flèche Droite`
* **Passer un tour (attente sur place)** : Touche `Espace`
* **Navigation dans les menus** : Touches `1` à `5`
* **Abandon en cours de partie** : Touche `C` ou touche `Échap`

> *Note : Les déplacements sont réactifs et instantanés. L'utilisation conjointe des touches classiques ZQSD, des flèches directionnelles et de la barre d'espace pour temporiser est supportée nativement.*

### Légende des symboles

| Symbole | Signification | Description |
| :---: | :--- | :--- |
| `#` | **Mur** | Infranchissable pour le joueur et l'ogre. |
| ` ` | **Couloir** | Espace de libre circulation. |
| `o` | **Joueur** | Votre personnage actuel. |
| `-` | **Porte de sortie** | Issue du labyrinthe (bloquée sans la clé `K`). |
| `K` | **Clé** | Obligatoire pour déverrouiller la sortie `-`. |
| `$` | **Trésor** | Bonus de score fixe (**+200 pts**). |
| `!` | **Piège** | Malus de score fixe (**-100 pts**). |
| `F` | **Fantôme** | Monstre spectral traversant les murs intérieurs (**-75 pts** au contact). |
| `O` | **Ogre** | Monstre patrouillant près des trésors (**-150 pts** au contact). |
| `·` | **Traînée** | Marque les 10 dernières positions traversées par le joueur. |

### Niveaux de difficulté
Le choix de la difficulté se fait lors de la création d'un labyrinthe :
* **1. Mode Facile** :
  * Labyrinthe mathématiquement parfait (arbre couvrant sans aucun cycle ni boucle).
  * Il n'existe qu'un unique chemin reliant chaque cellule à n'importe quelle autre.
  * Contient la clé, des bonus et des pièges, mais **aucun monstre**.
* **2. Mode Difficile** :
  * Labyrinthe imparfait : plusieurs murs intérieurs sont démolis pour créer des **chemins alternatifs et des boucles**.
  * Des **monstres** jalonnent le parcours et se déplacent à chacun de vos pas.

### Système de score et Top 10
* **Score initial** : Accordé à 1 000 points.
* **Rapidité** : Chaque déplacement consomme **1 point** (le score dépend de l'efficacité de vos pas, non du temps réel).
* **Objets ramassés** : Trésors (`+200 pts`), Pièges (`-100 pts`).
* **Attaques subies** : Contact avec un Fantôme (`-75 pts`), Contact avec un Ogre (`-150 pts`).
* **Score plancher** : Le score ne peut jamais devenir négatif (plafonné à 0).
* **Tableau** : À la fin d'une partie victorieuse, si votre score figure parmi les 10 meilleurs du labyrinthe, vous êtes invité à saisir votre pseudo. Le classement est sauvegardé dans `saves/<nom_du_labyrinthe>.score` et consultable depuis le menu principal (Option 4).

---

## Manuel du Développeur

### Architecture et modularité
Le code est conçu selon les principes du génie logiciel enseignés dans le cours d'ODL (Outils de Développement Logiciel), ainsi que des conseils apportés par des collègues de travail, complétés par des recherches internet :
* **Zéro variable globale** : Tout l'état du jeu est encapsulé dans des structures de données allouées dynamiquement et transmises en paramètres.
* **Const-correctness** : Protection en lecture seule systématique des paramètres (`const Maze *`, `const ScoreBoard *`, etc.).
* **Espaces de nommage par préfixe** : Chaque fonction publique est préfixée par son module (`maze_*`, `score_board_*`, `ui_*`, `monster_*`, `game_*`).

### Pointeurs de fonctions et monstres
Conformément aux exigences strictes de l'Étape 4, la mobilité de chaque type de monstre est modélisée par un **pointeur vers une fonction** :

```c
typedef void (*MonsterMoveFn)(struct Monster *monster, struct Maze *maze);
```

Chaque monstre possède deux paramètres clés :
* **Règle de mobilité (`move`)** :
  * `monster_move_ghost` : Le Fantôme traverse les murs intérieurs et se déplace vers le joueur dès qu'il pénètre son territoire.
  * `monster_move_ogre` : L'Ogre reste strictement dans les couloirs ouverts, patrouille à proximité des trésors (`$`) présents dans son secteur ou traque le joueur.
* **Étendue de mobilité (`territory_radius`)** :
  * Proportionnelle au nombre de points de pénalité infligés :
    * Ogre : forte pénalité (150 pts) $\rightarrow$ rayon d'action étendu (rayon 4).
    * Fantôme : pénalité modérée (75 pts) $\rightarrow$ rayon d'action plus restreint (rayon 2).

À chaque déplacement du joueur, la fonction `maze_update_monsters()` appelle dynamiquement `monster->move(monster, maze)` pour chaque entité active.

### Persistance et échappement des scores
Afin de préserver l'intégrité du fichier `.score` lors de l'utilisation de `fscanf("%s %d")` :
* Tout caractère `_` saisi dans un nom de joueur est **doublé** (`__`).
* Tout espace ` ` est encodé par un underscore simple `_`.
* À la lecture, le processus inverse est appliqué (`__` $\rightarrow$ `_`, `_` isolé $\rightarrow$ ` `).
* **Résultat** : Un pseudo tel que `"Jean Dupont"` ou `"Arthur_S"` est sauvegardé sous forme d'un jeton unique sans espace (`"Jean_Dupont"`, `"Arthur__S"`), éliminant tout risque de désynchronisation de lecture.

### Robustesse et rétrocompatibilité
* **Fallback pour labyrinthes sans clé** : Lors du chargement d'un fichier `.cfg` d'étape 1 ou 2 (qui ne possédait pas de clé `K`), `maze_load()` détecte l'absence de clé et en injecte automatiquement une sur une case couloir libre. Le labyrinthe reste ainsi jouable et terminable sans modification manuelle du fichier.
* **Lecture sécurisée des en-têtes** : La première ligne des fichiers `.cfg` est lue via `fgets()` et `sscanf()`, évitant les sauts de lignes résiduels des anciens formats.
* **Purge du buffer d'entrée** : Les dépassements de buffer dans `ui_prompt_string()` sont purgés, et `tcflush()` vide les frappes parasites avant l'affichage des menus.

### Gestion de la mémoire
* Aucune fuite mémoire : chaque structure allouée par `maze_create()` est détruite proprement par `maze_free()`, qui libère chaque ligne de la grille, le tableau des lignes, puis la structure hôte.
* Tous les tableaux temporaires générés lors des tirages aléatoires (`free_cells`, `walls`, `cell_ids`) sont libérés avant la sortie des fonctions.
* Validé sous **AddressSanitizer** et **UndefinedBehaviorSanitizer**.

### Tests unitaires (MinUnit)
Le projet intègre une suite de tests unitaires exhaustive basée sur le framework **MinUnit** :
* **71 tests unitaires** totalisant **2 116 assertions**, structurés en 5 modules :
  * `test_maze.c` : 29 tests (dimensions, Kruskal, sauvegarde/chargement, collisions, clés, fallback, temporisation sur place).
  * `test_monster.c` : 9 tests (déplacements, traversée des murs, conservation du rayon territorial).
  * `test_score.c` : 11 tests (tri par insertion, saturation à 10 scores, éviction, encodage des espaces).
  * `test_ui.c` : 13 tests (menus, formats de HUD, listings de dossiers, purge de buffer `stdin`).
  * `test_game.c` : 9 tests (résolution BFS, passage de tour, rejouabilité).

Exécution des tests :
```sh
make test
```

---

## Compilation et Exécution

### Prérequis
* Un compilateur C compatible C99 (ex: `gcc`, `clang`).
* GNU Make.
* Un environnement compatible POSIX (Linux, macOS, WSL sous Windows ou MSYS2).

### Compilation
Depuis la racine du projet :
```sh
make
```
Le binaire est généré dans `build/labyrinthe`.

### Exécution
```sh
./build/labyrinthe
```

---

## Commandes du Makefile

| Commande | Description |
| :--- | :--- |
| `make` ou `make all` | Compile le programme principal (`build/labyrinthe`). |
| `make test` | Compile et exécute la suite complète de 71 tests unitaires MinUnit. |
| `make zip` | Génère l'archive de rendu `labyrinthe.zip` prête à être remise (inclut les exécutables). |
| `make doc` | Génère la documentation HTML Doxygen dans `build/docs/html/`. |
| `make clean` | Supprime les fichiers objets intermédiaires (`build/obj/`). |
| `make fclean` | Nettoie complètement le dossier `build/` et l'archive `labyrinthe.zip`. |
| `make re` | Réexécute un nettoyage complet puis recompile le projet. |
| `make help` | Affiche l'aide détaillée des cibles disponibles. |

---

## Documentation Doxygen

Le code source est intégralement documenté au standard Doxygen (`@brief`, `@param`, `@return`, `@author`, `@file`).  
Pour générer la documentation au format HTML :
```sh
make doc
```
Ouvrez ensuite le fichier `build/docs/html/index.html` dans votre navigateur web favori.
