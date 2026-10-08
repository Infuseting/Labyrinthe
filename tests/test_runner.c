#include <stdio.h>

int run_utils_tests(void);
int run_maze_tests(void);

int main(void) {
    int total_failures = 0;

    printf("\n=== Execution des tests : Module Utils ===\n");
    total_failures += run_utils_tests();

    printf("\n=== Execution des tests : Module Maze ===\n");
    total_failures += run_maze_tests();

    printf("\n==========================================\n");
    if (total_failures > 0) {
        printf("Resultat final : %d echec(s) detecte(s).\n", total_failures);
        printf("==========================================\n");
        return 1;
    }

    printf("Resultat final : Tous les tests ont reussi !\n");
    printf("==========================================\n");
    return 0;
}
