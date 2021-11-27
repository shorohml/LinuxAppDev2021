#include "config.h"
#include <ctype.h>
#include <errno.h>
#ifdef WITH_READLINE
#include <readline/readline.h>
#endif
#include <rhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_ALGOS 4

void free_resources(char **line, size_t *n) {
    free(*line);
    *line = NULL;
    *n = 0;
}

void str_to_upper(char *s) {
    while (*s) {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

int main() {
    rhash context;
    unsigned char digest[64];
    char output[130];
    size_t n = 0;
    char *line = NULL, *algo_name, *filepath;
    const char *usage_msg = "usage: hash_name {\"string\"|filepath}";
    int algo_options[NUM_ALGOS] = {RHASH_MD4, RHASH_MD5, RHASH_SHA1, RHASH_TTH};
    int algo, format;

    rhash_library_init(); /* initialize static data */

#ifdef WITH_READLINE
    while (NULL != (line = readline("> "))) {
#else
    for (printf("> "); - 1 != getline(&line, &n, stdin); printf("> ")) {
        /* mimic readline behavior */
        if (!(isatty(fileno(stdin)))) {
            printf("%s", line);
        }
#endif
        /* read command arguments */
        if (NULL == (algo_name = strtok(line, " "))) {
            printf("%s\n", usage_msg);
            free_resources(&line, &n);
            continue;
        }
        if (NULL == (filepath = strtok(NULL, " "))) {
            printf("%s\n", usage_msg);
            free_resources(&line, &n);
            continue;
        }
        if ('\n' == filepath[0]) {
            printf("%s\n", usage_msg);
            free_resources(&line, &n);
            continue;
        }

#ifndef WITH_READLINE
        /* Remove \n from filepath */
        filepath[strlen(filepath) - 1] = 0;
#endif

        /* select hashing algorithm */
        algo = -1;
        for (int i = 0; i < NUM_ALGOS; ++i) {
            if (islower(algo_name[0])) {
                format = RHPR_BASE64;
            } else {
                format = RHPR_HEX;
            }
            str_to_upper(algo_name);
            if (!strcmp(algo_name, rhash_get_name(algo_options[i]))) {
                algo = algo_options[i];
                break;
            }
        }
        if (-1 == algo) {
            printf("unsupported hash: %s\n", algo_name);
            free_resources(&line, &n);
            continue;
        }

        context = rhash_init(algo);
        if (!context) {
            fprintf(stderr, "couldn't initialize rhash context\n");
            continue;
        }

        /* compute hash */
        if (filepath[0] == '"') {
            /* it's actually a string, remove first symbol */
            ++filepath;

            rhash_update(context, filepath, 1);
            rhash_final(context, NULL);

            rhash_print(output, context, algo, format);
            printf("%s\n", output);
        } else {
            int res = rhash_file(algo, filepath, digest);
            if (res < 0) {
                fprintf(stderr, "%s: %s\n", filepath, strerror(errno));
                rhash_free(context);
                free_resources(&line, &n);
                continue;
            }

            /* convert binary digest to hexadecimal string */
            rhash_print_bytes(output, digest, rhash_get_digest_size(algo),
                                format);
            printf("%s\n", output);
        }

        rhash_free(context);
        free_resources(&line, &n);
    }
    return 0;
}