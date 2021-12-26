#include <argp.h>
#include <ctype.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UPPER 100
#define _(STRING) gettext(STRING)
#define LOCALE_PATH "po"

const char *argp_program_version = "guess 0.0.1";
const char *argp_program_bug_address = "<no-reply>";

/* Program documentation. */
static char doc[] = "Program for guessing number from 1 to 100.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
    {"roman", 'r', 0, 0, "Work with romain numbers"}, {0}};

struct arguments {
    int roman;
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
    case 'r':
        arguments->roman = 1;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
            argp_usage(state);

        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

static char *to_lower(char *s) {
    for (char *p = s; *p; ++p) {
        *p = tolower(*p);
    }
    return s;
}

const char *roman_table[UPPER] = {
    "I",       "II",     "III",     "IV",       "V",      "VI",      "VII",
    "VIII",    "IX",     "X",       "XI",       "XII",    "XIII",    "XIV",
    "XV",      "XVI",    "XVII",    "XVIII",    "XIX",    "XX",      "XXI",
    "XXII",    "XXIII",  "XXIV",    "XXV",      "XXVI",   "XXVII",   "XXVIII",
    "XXIX",    "XXX",    "XXXI",    "XXXII",    "XXXIII", "XXXIV",   "XXXV",
    "XXXVI",   "XXXVII", "XXXVIII", "XXXIX",    "XL",     "XLI",     "XLII",
    "XLIII",   "XLIV",   "XLV",     "XLVI",     "XLVII",  "XLVIII",  "XLIX",
    "L",       "LI",     "LII",     "LIII",     "LIV",    "LV",      "LVI",
    "LVII",    "LVIII",  "LIX",     "LX",       "LXI",    "LXII",    "LXIII",
    "LXIV",    "LXV",    "LXVI",    "LXVII",    "LXVIII", "LXIX",    "LXX",
    "LXXI",    "LXXII",  "LXXIII",  "LXXIV",    "LXXV",   "LXXVI",   "LXXVII",
    "LXXVIII", "LXXIX",  "LXXX",    "LXXXI",    "LXXXII", "LXXXIII", "LXXXIV",
    "LXXXV",   "LXXXVI", "LXXXVII", "LXXXVIII", "LXXXIX", "XC",      "XCI",
    "XCII",    "XCIII",  "XCIV",    "XCV",      "XCVI",   "XCVII",   "XCVIII",
    "XCIX",    "C"};

static const char *to_roman(int num) {
    if (num < 1 || num > UPPER) {
        return "";
    }
    return roman_table[num - 1];
}

int main(int argc, char **argv) {
    int lower = 0, upper = UPPER, half;
    size_t n;
    char *answer = NULL;
    struct arguments arguments;

    /* Default values. */
    arguments.roman = 0;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALE_PATH);
    textdomain("guess");

    if (arguments.roman) {
        printf(_("Guess a number from I to %s\n"), to_roman(UPPER));
    } else {
        printf(_("Guess a number from 1 to %d\n"), UPPER);
    }
    while (lower < upper - 1) {
        half = (lower + upper) / 2;
        if (arguments.roman) {
            printf(_("Is the number greater than %s? (y/n)\n"), to_roman(half));
        } else {
            printf(_("Is the number greater than %d? (y/n)\n"), half);
        }

        getline(&answer, &n, stdin);
        answer = to_lower(answer);

        if (!strcmp(answer, _("y\n")) || !strcmp(answer, _("yes\n"))) {
            lower = half;
        } else if (!strcmp(answer, _("n\n")) || !strcmp(answer, _("no\n"))) {
            upper = half;
        }

        free(answer);
        answer = NULL;
    }
    if (arguments.roman) {
        printf(_("Your number is %s\n"), to_roman(upper));
    } else {
        printf(_("Your number is %d\n"), upper);
    }
}