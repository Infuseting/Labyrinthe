CC       = gcc
CFLAGS   = -Wall -Wextra -pedantic -std=c99 -g
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Iincludes -Iincludes/minunit -Iinclude
LDLIBS   = -lm

SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj

# Fichiers sources et objets
SRC      = $(wildcard $(SRC_DIR)/*.c)
OBJ      = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Cibles (les executables sont generes dans build/)
TARGET      = $(BUILD_DIR)/labyrinthe
TEST_TARGET = $(BUILD_DIR)/test_runner

# Objets pour les tests : inclut tous les modules de src/ SAUF main.o
OBJ_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
TEST_SRC    = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ    = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRC))

.PHONY: all test clean fclean re help

all: $(TARGET)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
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

clean:
	rm -rf $(OBJ_DIR) obj

fclean: clean
	rm -rf $(BUILD_DIR)
	rm -f labyrinthe test_runner

re: fclean all

help:
	@echo "Usage : make [cible]"
	@echo "  all     : Compile le projet principal ($(TARGET))"
	@echo "  test    : Compile et execute les tests unitaires MinUnit ($(TEST_TARGET))"
	@echo "  clean   : Supprime les fichiers objets temporaires"
	@echo "  fclean  : Supprime le dossier $(BUILD_DIR)/ et tous les executables"
	@echo "  re      : Recompile tout depuis zero"
	@echo "  help    : Affiche cette aide"
