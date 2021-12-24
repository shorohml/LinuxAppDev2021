#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

char *to_lower(char *s) {
    for (char *p = s; *p; ++p) {
        *p  = tolower(*p);
    }
    return s;
}

int main() {
    int lower = 1, upper = 100, half;
    size_t n;
    char *answer = NULL;

    printf("Guess a number from 1 to 100\n");
    while (lower < upper - 1) {
        half = (lower + upper) / 2;
        printf("is the number greater than %d? (y/n)\n", half);
        getline(&answer, &n, stdin);
        answer = to_lower(answer);

        if (!strcmp(answer, "y\n") || !strcmp(answer, "yes\n")) {
            lower = half;
        } else if (!strcmp(answer, "n\n") || !strcmp(answer, "no\n")) {
            upper = half;
        }

        free(answer);
        answer = NULL;
    }
    printf("Your number is %d\n", upper);
}