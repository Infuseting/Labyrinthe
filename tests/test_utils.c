#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <minunit.h>
#include <utils.h>

static const char *TEST_INPUT_FILE = "build/test_input.tmp";

static void set_stdin(const char *input_text) {
    FILE *fp = fopen(TEST_INPUT_FILE, "w");
    if (fp != NULL) {
        fputs(input_text, fp);
        fclose(fp);
    }
    if (freopen(TEST_INPUT_FILE, "r", stdin) == NULL) {
        perror("freopen failed");
    }
}

static void test_teardown(void) {
    remove(TEST_INPUT_FILE);
}

/* --- Tests pour getChoice --- */

MU_TEST(test_getChoice_valid_middle) {
    set_stdin("3\n");
    int choice = 0;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(ok);
    mu_assert_int_eq(3, choice);
}

MU_TEST(test_getChoice_valid_lower_bound) {
    set_stdin("1\n");
    int choice = 0;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(ok);
    mu_assert_int_eq(1, choice);
}

MU_TEST(test_getChoice_valid_upper_bound) {
    set_stdin("4\n");
    int choice = 0;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(ok);
    mu_assert_int_eq(4, choice);
}

MU_TEST(test_getChoice_out_of_range_below) {
    set_stdin("0\n");
    int choice = -1;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(!ok);
}

MU_TEST(test_getChoice_out_of_range_above) {
    set_stdin("5\n");
    int choice = -1;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(!ok);
}

MU_TEST(test_getChoice_negative_value) {
    set_stdin("-10\n");
    int choice = -1;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(!ok);
}

MU_TEST(test_getChoice_invalid_text) {
    set_stdin("invalid\n");
    int choice = -1;
    bool ok = getChoice(&choice, 1, 4);
    mu_check(!ok);
}

MU_TEST(test_getChoice_recovery_after_invalid_input) {
    /* Verifie que clearInputBuffer a bien vide la ligne invalide
       et permet a la saisie suivante de fonctionner */
    set_stdin("erreur\n2\n");
    int choice1 = -1;
    bool ok1 = getChoice(&choice1, 1, 4);
    mu_check(!ok1);

    int choice2 = 0;
    bool ok2 = getChoice(&choice2, 1, 4);
    mu_check(ok2);
    mu_assert_int_eq(2, choice2);
}

/* --- Tests pour clearInputBuffer --- */

MU_TEST(test_clearInputBuffer_discards_until_newline) {
    set_stdin("unwanted characters\nnext_line\n");
    clearInputBuffer();
    int next_char = getchar();
    mu_assert_int_eq('n', next_char);
}

MU_TEST(test_clearInputBuffer_on_empty_or_eof) {
    set_stdin("");
    clearInputBuffer();
    mu_check(feof(stdin));
}

/* --- Tests pour clearScreen --- */

MU_TEST(test_clearScreen_execution) {
    /* Verifie que clearScreen s'execute sans planter */
    clearScreen();
    mu_check(true);
}

/* --- Suite de tests --- */

MU_TEST_SUITE(utils_test_suite) {
    MU_SUITE_CONFIGURE(NULL, test_teardown);

    MU_RUN_TEST(test_getChoice_valid_middle);
    MU_RUN_TEST(test_getChoice_valid_lower_bound);
    MU_RUN_TEST(test_getChoice_valid_upper_bound);
    MU_RUN_TEST(test_getChoice_out_of_range_below);
    MU_RUN_TEST(test_getChoice_out_of_range_above);
    MU_RUN_TEST(test_getChoice_negative_value);
    MU_RUN_TEST(test_getChoice_invalid_text);
    MU_RUN_TEST(test_getChoice_recovery_after_invalid_input);
    MU_RUN_TEST(test_clearInputBuffer_discards_until_newline);
    MU_RUN_TEST(test_clearInputBuffer_on_empty_or_eof);
    MU_RUN_TEST(test_clearScreen_execution);
}

int main(void) {
    MU_RUN_SUITE(utils_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
