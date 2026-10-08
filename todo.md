# TODO - Projet Generateur de Labyrinthe
---

## 1. Transversal et Infrastructure de Base

- [X] **Architecture du projet**
  - [X] Mettre en place l'arborescence (`src/`, `include/`, `tests/`, `saves/`, `doc/`).
  - [X] Rediger le `Makefile` (cibles `all`, `clean`, `test`, drapeaux `-Wall -Wextra -pedantic -std=c99`).
  - [X] Integrer la bibliotheque de tests unitaires `MinUnit` (`minunit.h`).
- [ ] **Conventions et qualite (ODL)**
  - [X] Appliquer une norme de nommage stricte et coherente.
  - [X] Documenter les prototypes dans les fichiers `.h` (format Doxygen : `@brief`, `@param`, `@return`).
  - [X] Verifier systematiquement l'absence de fuites memoire avec `valgrind`.

--- 

## 2. Etape 1 : Generateur de Labyrinthe Parfait (Taille Fixe 11x25)

- [ ] **Modele de donnees et memoire**
  - [ ] Definir les constantes (`HAUTEUR = 11`, `LARGEUR = 25`).
  - [ ] Definir les symboles (`#` pour les murs, ` ` pour les chemins, `o` pour le joueur, `-` pour la sortie).
  - [ ] Definir la structure `Maze` (matrice 2D, coordonnees de depart et d'arrivee).
  - [ ] Implementer l'allocation et la liberation de la grille.
- [ ] **Algorithme de generation**
  - [ ] Implementer l'algorithme fourni en annexe.
  - [ ] Positionner l'entree en haut a gauche `(1, 1)` et la sortie en bas a droite.
  - [ ] Positionner la porte de sortie `-`.
- [ ] **Affichage et execution**
  - [ ] Creer la fonction d'affichage console de la grille.
  - [ ] Positionner le joueur `o` sur sa case initiale.
  - [ ] Ecrire un `main` minimal validant l'etape 1.
- [ ] **Tests et compte-rendu**
  - [ ] Ecrire les tests MinUnit (dimensions, continuite des bordures, presence entree/sortie).
  - [ ] Rediger la section Etape 1 du compte-rendu (explication technique de l'algorithme).
  - [ ] Validation en seance de TP.

---

## 3. Etape 2 : Menu, Fichiers .cfg et Deplacements

- [ ] **Menu principal**
  - [ ] Mettre en place la boucle du menu (Creer, Charger, Jouer, Quitter).
  - [ ] Securiser la saisie utilisateur (purge de `stdin`, filtrage des entres invalides).
- [ ] **Creation dynamique (2.1)**
  - [ ] Demander le nom du labyrinthe.
  - [ ] Demander la hauteur et la largeur (verification : nombres impairs).
  - [ ] Adapter l'algorithme de generation aux dimensions variables.
  - [ ] Charger automatiquement en memoire le labyrinthe cree.
- [ ] **Gestion des fichiers .cfg (2.1 et 2.2)**
  - [ ] Definir le format texte de stockage `.cfg` (dimensions, matrice).
  - [ ] Implementer la sauvegarde dans `<nom>.cfg`.
  - [ ] Implementer la lecture et le chargement depuis `<nom>.cfg`.
  - [ ] Gerer les erreurs (fichier introuvable, format incorrect).
  - [ ] Lister les fichiers de sauvegarde disponibles.
- [ ] **Moteur de jeu (2.3)**
  - [ ] Suivre la position courante du joueur `(x, y)`.
  - [ ] Gerer les deplacements avec `z`, `q`, `s`, `d` puis `Entree`.
  - [ ] Implementer la detection des collisions contre les murs `#`.
  - [ ] Mettre a jour l'affichage apres chaque coup valide.
  - [ ] Detecter l'arrivee sur la case de sortie `-` et retourner au menu.
- [ ] **Tests et compte-rendu**
  - [ ] Ecrire les tests MinUnit sur les E/S fichiers (sauvegarde puis relecture identique).
  - [ ] Ecrire les tests MinUnit des deplacements et collisions.
  - [ ] Rediger la section Etape 2 du compte-rendu.
  - [ ] Validation en seance de TP.

---

## 4. Etape 3 : Objets, Cle, Score et Highscores (.score)

- [ ] **Objets et inventaire**
  - [ ] Definir les constantes et symboles : cle, tresors (bonus +X), pieges (malus -Y).
  - [ ] Ajouter l'etat de possession de la cle a la structure de jeu.
- [ ] **Placement aleatoire**
  - [ ] Identifier les cases de chemin disponibles (hors entree et sortie).
  - [ ] Disposer aleatoirement la cle, les tresors et les pieges.
  - [ ] Mettre a jour le format `.cfg` pour sauvegarder et charger ces elements.
- [ ] **Regles de jeu et calcul du score**
  - [ ] Bloquer la sortie `-` tant que la cle n'est pas recuperee.
  - [ ] Debloquer la sortie des ramassage de la cle.
  - [ ] Gerer le ramassage des bonus et pieges (la case devient un couloir vide).
  - [ ] Calculer le score en continu (decroissance par pas deplacement, ajout/retrait des bonus/malus).
  - [ ] Afficher le score en temps reel a chaque mouvement.
- [ ] **Classement (.score)**
  - [ ] Definir la structure representant une entree de score (nom, score, deplacements).
  - [ ] Implementer la lecture, le tri et l'ecriture du top 10 dans `<nom>.score`.
  - [ ] Proposer la saisie du nom si le score final se classe dans le top 10.
  - [ ] Ajouter l'option dans le menu principal : visualiser les 10 meilleurs scores.
- [ ] **Tests et compte-rendu**
  - [ ] Ecrire les tests MinUnit sur l'algorithme de calcul du score et le tri du top 10.
  - [ ] Ecrire les tests MinUnit sur la persistance des `.score`.
  - [ ] Rediger la section Etape 3 du compte-rendu.
  - [ ] Validation en seance de TP.

---

## 5. Etape 4 : Niveaux de Difficulte, Monstres et Pointeurs de Fonctions

- [ ] **Niveaux de difficulte**
  - [ ] Ajouter le choix du mode a la creation : Facile (parfait) ou Difficile (imparfait).
  - [ ] En mode difficile : abattre des murs interieurs au hasard pour introduire des cycles et chemins multiples.
- [ ] **Structure des monstres et pointeurs de fonctions**
  - [ ] Definir la structure `Monster` :
    - [ ] Position `(x, y)`.
    - [ ] Type et symbole specifique.
    - [ ] Zone de mobilite (proportionnelle aux points de penalite).
    - [ ] Valeur des penalites.
    - [ ] Pointeur de fonction : `void (*deplacer)(Monster *self, Maze *maze, const Player *player)`.
- [ ] **Comportements d'IA**
  - [ ] Fantome : fonction de deplacement autorisant le franchissement des murs `#`.
  - [ ] Ogre : fonction de deplacement avec patrouille limitee autour des bonus/tresors.
- [ ] **Mise a jour du tour de jeu**
  - [ ] Executer le deplacement de chaque monstre via son pointeur de fonction apres chaque action du joueur.
  - [ ] Gerer la rencontre Joueur / Monstre (application de la penalite, repli ou disparition).
- [ ] **Tests et compte-rendu**
  - [ ] Ecrire les tests MinUnit verifiant l'execution correcte des pointeurs de fonctions de deplacement.
  - [ ] Ecrire les tests MinUnit validant l'existence de cycles en mode difficile.
  - [ ] Rediger la section Etape 4 du compte-rendu.
  - [ ] Validation en seance de TP.

---

## 6. Finalisation des Livrables

- [ ] **Revue du code source**
  - [ ] Compilation stricte sans aucun avertissement (`-Wall -Wextra -Werror`).
  - [ ] Passage sous `valgrind` pour garantir l'absence totale de fuites memoire.
  - [ ] Modularite validee (encapsulation, fonctions `static` internes).
- [ ] **Redaction du compte-rendu**
  - [ ] Manuel joueur : compilation, execution, touches, explications des regles et symboles.
  - [ ] Manuel developpeur :
    - [ ] Bilan de ce qui a ete fait et des fonctionnalites restantes.
    - [ ] Architecture logicielle et points cles (pointeurs de fonctions, persistance, generation).
    - [ ] Limitations et bugs connus.
- [ ] **Constitution de l'archive de rendu**
  - [ ] Rassembler les sources, le `Makefile`, les tests MinUnit et le compte-rendu.
  - [ ] Verifier la decompression et la compilation complete sur un environnement propre.