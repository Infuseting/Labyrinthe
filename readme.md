# Jeu du Labyrinthe

## Présentation du projet

Ce projet consiste en la conception et la réalisation complète en **langage C (norme C99)** d'un générateur de labyrinthes et d'un jeu d'exploration en mode console.  

Il valide l'ensemble des 4 étapes décrites dans le cahier des charges :
1. **Étape 1** : Générateur de labyrinthe parfait (taille fixe 11×25) via l'algorithme de fusion aléatoire de chemins (Kruskal) et affichage en mode console.
2. **Étape 2** : Menu interactif, choix des dimensions impaires, sauvegarde/chargement au format `.cfg` et déplacements au clavier.
3. **Étape 3** : Objets interactifs (clé déverrouillant la porte, bonus de points, malus de pièges), calcul du score en fonction des pas et classement persistant du Top 10 (`.score`).
4. **Étape 4** : Deux niveaux de difficulté (Facile/Difficile), génération de cycles (chemins multiples) et monstres autonomes (Fantômes et Ogres) régis par des **pointeurs vers des fonctions**.
   
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
