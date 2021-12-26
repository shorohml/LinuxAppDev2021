#include <argp.h>
#include <ctype.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UPPER 100                 /**< Maximum number to guess */
#define _(STRING) gettext(STRING) /**< Translation */
#define LOCALE_PATH "po"          /**< Locate directory */

const char *argp_program_version = "guess 0.0.1"; /**< Program version. */
const char *argp_program_bug_address =
    "<no-reply>"; /**<  Bug report address. */

struct arguments {
    int roman;   /**< 1 to use roman numbers, 0 to use arabic numbers */
    int convert; /**< 1 to run in convert mode, 0 to guess number */
};

/**
 * Parse a single option \p key with optional argument \p arg  when state is \p
 * state
 *
 * @param key Commandline option
 * @param arg Optional key argument
 * @param state INternal parser state
 *
 * @returns Error state
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
    case 'r':
        arguments->roman = 1;
        break;

    case 'c':
        arguments->convert = 1;
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

/**
 * Convert every character in string \p s to lower regiter.
 *
 * @param s Input string
 *
 * @returns Result string
 */
static char *to_lower(char *s) {
    for (char *p = s; *p; ++p) {
        *p = tolower(*p);
    }
    return s;
}

/**
 * Convert every character in string \p s to upper regiter.
 *
 * @param s Input string
 *
 * @returns Result string
 */
static char *to_upper(char *s) {
    for (char *p = s; *p; ++p) {
        *p = toupper(*p);
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
    "XCIX",    "C"}; /**< Table with roman numbers from 1 to UPPER. */

/**
 * Convert arabic number \p num to roman number.
 *
 * @param num Integer number
 *
 * @returns String with roman number
 */
static const char *to_roman(int num) {
    if (num < 1 || num > UPPER) {
        return "";
    }
    return roman_table[num - 1];
}

/**
 * Convert one symbol from roman number \p digit to integer.
 *
 * @param digit Roman number symbol
 *
 * @returns Integer value (-1 in case of invalid symbol)
 */
static int from_roman_digit(const char digit) {
    switch (digit) {
    case 'I':
        return 1;
    case 'V':
        return 5;
    case 'X':
        return 10;
    case 'L':
        return 50;
    case 'C':
        return 100;
    case 'D':
        return 500;
    case 'M':
        return 1000;
    }
    return -1;
}

/**
 * Convert roman number \p roman to arabic number.
 *
 * @param roman Roman number
 *
 * @returns Arabic number (-1 in case of invalid roman number)
 */
static int from_roman(const char *roman) {
    if (!(*roman)) {
        return -1;
    }

    int prev;
    if (-1 == (prev = from_roman_digit(roman[0]))) {
        return -1;
    }

    int curr = prev, sum = 0;
    size_t len = strlen(roman);

    for (size_t i = 1; i < len; ++i) {
        if ('\n' == roman[i]) {
            sum += prev;
            return sum;
        }

        if (-1 == (curr = from_roman_digit(roman[i]))) {
            return -1;
        }

        if (prev >= curr) {
            sum += prev;
        } else {
            sum -= prev;
        }

        prev = curr;
    }
    sum += curr;
    return sum;
}

int main(int argc, char **argv) {
    int lower = 0, upper = UPPER, half;
    size_t n;
    char *answer = NULL;
    struct arguments arguments;

    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALE_PATH);
    textdomain("guess");

    struct argp_option options[] = {
        {"roman", 'r', 0, 0, _("Print romain numbers")},
        {"convert", 'c', 0, 0, _("Convert roman number to arabic")},
        {0}}; /* The options we understand. */
    struct argp argp = {
        options, parse_opt, "",
        _("Program for guessing number from 1 to 100.")}; /* Our argp parser. */

    /* Default values. */
    arguments.roman = 0;
    arguments.convert = 0;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    textdomain("libc");
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    textdomain("guess");

    if (arguments.convert) {
        int arabic;

        printf(_("Enter roman number:\n"));

        if (-1 == getline(&answer, &n, stdin)) {
            perror("readline");
            return -1;
        }
        answer = to_upper(answer);

        if (-1 == (arabic = from_roman(answer))) {
            printf(_("Invalid roman number\n"));
            free(answer);
            answer = NULL;
            return -1;
        }

        printf(_("Arabic number: %d\n"), from_roman(answer));

        free(answer);
        answer = NULL;
        return 0;
    }

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

        if (-1 == getline(&answer, &n, stdin)) {
            perror("readline");
            return -1;
        }
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
    return 0;
}