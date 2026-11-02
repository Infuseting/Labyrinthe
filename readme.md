# Labyrinthe

Projet en C99, compilé avec GCC et Make. Les exécutables sont générés dans `build/`.

## Prérequis

- GCC et GNU Make.
- Un environnement Unix ou compatible (Linux, macOS, WSL ou MSYS2), pour les commandes `mkdir -p` et `rm` du Makefile.

## Compilation et exécution

Depuis la racine du projet :

```sh
make
./build/labyrinthe
```

`make` (ou `make all`) compile les fichiers `src/*.c` avec les options `-Wall -Wextra -pedantic -std=c99 -g` et lie la bibliothèque mathématique (`-lm`).

## Tests unitaires

```sh
make test
```

Compile les tests MinUnit de `tests/*.c` avec les modules du projet, sauf `src/main.c`, puis exécute `build/test_runner`.

## Commandes utiles

| Commande | Action |
| --- | --- |
| `make clean` | Supprime les fichiers objets (`build/obj/` et l'ancien dossier `obj/`). |
| `make fclean` | Supprime aussi `build/` et les anciens exécutables à la racine. |
| `make re` | Nettoie complètement puis recompile le programme principal. |
| `make help` | Affiche l'aide du Makefile. |

## Organisation

- `src/` : sources du programme, dont le point d'entrée `main.c`.
- `tests/` : sources des tests unitaires.
- `include/` : fichiers d'en-tête, dont `minunit.h`.
- `build/obj/` : fichiers objets générés.
- `build/` : exécutables générés.
