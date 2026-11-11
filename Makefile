CC       = gcc
CFLAGS   = -Wall -Wextra -pedantic -std=c99 -g
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Iinclude
LDLIBS   = -lm

SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj

# Fichiers sources et objets
SRC      = $(wildcard $(SRC_DIR)/*.c)
OBJ      = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
INCLUDES = $(wildcard include/*.h)

# Cibles (les executables sont generes dans build/)
TARGET      = $(BUILD_DIR)/labyrinthe
TEST_TARGET = $(BUILD_DIR)/test_runner
ZIP_NAME    = labyrinthe.zip

# Objets pour les tests : inclut tous les modules de src/ SAUF main.o
OBJ_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
TEST_SRC    = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ    = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRC))

.PHONY: all test doc clean fclean re help zip

all: $(TARGET)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(INCLUDES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilation et execution des tests unitaires
test: $(TEST_TARGET)
	@echo ""
	@echo "=== Execution des tests unitaires MinUnit ==="
	@./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(OBJ_NO_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

doc: | $(BUILD_DIR)
	doxygen Doxyfile

# Creation de l'archive ZIP pour le rendu (inclut les executables, exclut .git)
zip: all
	@echo "Creation de l'archive $(ZIP_NAME)..."
	@rm -f $(ZIP_NAME)
	@if command -v zip >/dev/null 2>&1; then \
		zip -r $(ZIP_NAME) . -x ".git/*" -x ".git" -x "$(ZIP_NAME)"; \
	elif command -v python3 >/dev/null 2>&1; then \
		python3 -c "import os, zipfile; z = zipfile.ZipFile('$(ZIP_NAME)', 'w', zipfile.ZIP_DEFLATED); [z.write(os.path.join(r, f), os.path.relpath(os.path.join(r, f), '.')) for r, d, fs in os.walk('.') if not (r == './.git' or r.startswith('./.git/') or r == '.git' or r.startswith('.git/')) for f in sorted(fs) if f != '$(ZIP_NAME)']; z.close()"; \
	else \
		echo "Erreur : aucun utilitaire ('zip' ou 'python3') disponible pour creer l'archive."; exit 1; \
	fi
	@echo "Archive $(ZIP_NAME) creee avec succes."

clean:
	rm -rf $(OBJ_DIR) obj

fclean: clean
	rm -rf $(BUILD_DIR)
	rm -f labyrinthe test_runner $(ZIP_NAME)

re: fclean all

help:
	@echo "Usage : make [cible]"
	@echo "  all     : Compile le projet principal ($(TARGET))"
	@echo "  test    : Compile et execute les tests unitaires MinUnit ($(TEST_TARGET))"
	@echo "  doc     : Genere la documentation Doxygen dans $(BUILD_DIR)/docs"
	@echo "  zip     : Genere l'archive de rendu $(ZIP_NAME) (tout sauf .git)"
	@echo "  clean   : Supprime les fichiers objets temporaires"
	@echo "  fclean  : Supprime le dossier $(BUILD_DIR)/ et tous les executables"
	@echo "  re      : Recompile tout depuis zero"
	@echo "  help    : Affiche cette aide"