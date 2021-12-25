#include <ctype.h>
#include <libgen.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UPPER 100

#define _(STRING) gettext(STRING)
#define LOCALE_PATH "."

char *to_lower(char *s) {
    for (char *p = s; *p; ++p) {
        *p = tolower(*p);
    }
    return s;
}

int main(int argc, char **argv) {
    int lower = 1, upper = UPPER, half;
    size_t n;
    char *answer = NULL, *dir;

    dir = dirname(realpath(argv[0], NULL));
    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALE_PATH);
    textdomain("guess");

    printf(_("Guess a number from 1 to %d\n"), UPPER);
    while (lower < upper - 1) {
        half = (lower + upper) / 2;
        printf(_("Is the number greater than %d? (y/n)\n"), half);

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
    printf(_("Your number is %d\n"), upper);
}