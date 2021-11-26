#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t ERRBUF_SIZE = 128;
const size_t NMATCH = 128;

/* Replace \\ with \ in given string.

Allocates memory, so you must call free on the returned pointer.*/
char *process_double_backslash(const char *s) {
    int len, count, res_i = 0, end;
    char *res;

    len = strlen(s);
    if (NULL == (res = malloc(sizeof(char) * (len + 1)))) {
        return NULL;
    }

    for (int i = 0; i < len; ++i) {
        /* process \\ */
        if (s[i] == '\\') {
            count = 0;
            while (i < len && s[i] == '\\') {
                ++i;
                ++count;
            }
            --i;
            end = count / 2 + count % 2;
            for (int j = 0; j < end; ++j)
                res[res_i++] = '\\';
        } else {
            res[res_i++] = s[i];
        }
    }
    res[res_i] = 0;
    return res;
}

/* Check correctness of sub links.

If some link is incorrect, return -1, else return number of links. */
int check_sub_links(const char *subs, size_t nsub) {
    char *end;
    const char *p;
    long num;
    int n_links = 0;

    p = subs;
    while (*p) {
        if (*p == '\\') {
            /* get following number */
            num = strtol(p + 1, &end, 10);
            if (end == p + 1) {
                ++p;
                continue;
            }
            if (num < 0 || (size_t)num > nsub) {
                return -1;
            }
            ++n_links;
            p = end;
        } else {
            ++p;
        }
    }
    return n_links;
}

/* Link in a substitution string in a form of "\{number}" */
struct SubLink {
    const char *pbegin;
    const char *pend;
    size_t len;
    size_t num;
};

/* Find all links in a substitution string in a form of "\{number}".

Allocates memory, so you must call free on the returned pointer.*/
struct SubLink *parse_sub_links(const char *subs, int n_links) {
    struct SubLink *res;
    char *end;
    const char *p;
    long num;
    int links_count = 0;

    if (NULL == (res = malloc(sizeof(struct SubLink) * n_links))) {
        return NULL;
    }

    p = subs;
    while (*p) {
        if (*p == '\\') {
            /* get following number and fill out SubLink */
            if (links_count >= n_links) {
                free(res);
                return NULL;
            }
            num = strtol(p + 1, &end, 10);
            if (end == p + 1) {
                ++p;
                continue;
            }
            if (num < 0) {
                free(res);
                return NULL;
            }
            res[links_count++] = (struct SubLink){p, end, end - p, (size_t)num};
            p = end;
        } else {
            ++p;
        }
    }
    if (links_count != n_links) {
        free(res);
        return NULL;
    }
    return res;
}

/* Fill out all \"{number}" in the substitution string

Allocates memory, so you must call free on the returned pointer.*/
char *substitute(struct SubLink *sub_links, int n_links, const char *subs,
                 const char *orig, regmatch_t *pmatch) {
    size_t subs_len = strlen(subs);
    size_t res_len = 0;
    char *res;

    if (!n_links) {
        return strdup(subs);
    }

    for (int i = 0; i < n_links; ++i) {
        if (0 == i) {
            res_len += sub_links[i].pbegin - subs;
        } else {
            res_len += sub_links[i].pbegin - sub_links[i - 1].pend;
        }
        res_len +=
            pmatch[sub_links[i].num].rm_eo - pmatch[sub_links[i].num].rm_so;
    }
    res_len += (subs + subs_len) - sub_links[n_links - 1].pend;

    if (NULL == (res = malloc(sizeof(char) * (res_len + 1)))) {
        return NULL;
    }

    int res_j = 0;
    for (int i = 0; i < n_links; ++i) {
        if (0 == i) {
            for (int j = 0; j < sub_links[i].pbegin - subs; ++j) {
                res[res_j++] = subs[j];
            }
        } else {
            for (int j = 0; j < sub_links[i].pbegin - sub_links[i - 1].pend;
                 ++j) {
                res[res_j++] = *(sub_links[i - 1].pend + j);
            }
        }
        size_t part_len =
            pmatch[sub_links[i].num].rm_eo - pmatch[sub_links[i].num].rm_so;
        for (size_t j = 0; j < part_len; ++j) {
            res[res_j++] = *(orig + pmatch[sub_links[i].num].rm_so + j);
        }
    }
    res[res_len] = 0;
    return res;
}

int main(int argc, char **argv) {
    regex_t preg;
    regmatch_t *pmatch;
    regoff_t shift;
    int errcode, n_links;
    char *errbuf, *res, *tmp_res, *subs, *subs_res;
    size_t i, res_len, subs_len, orig_len;
    struct SubLink *sub_links;

    if (argc != 4) {
        fprintf(stderr, "Usage: ./esub regexp substitution string\n");
        return -1;
    }

    if (NULL == (errbuf = malloc(sizeof(char) * ERRBUF_SIZE))) {
        perror("");
        return errno;
    }
    if ((errcode = regcomp(&preg, argv[1], REG_EXTENDED))) {
        regerror(errcode, &preg, errbuf, ERRBUF_SIZE);
        fprintf(stderr, "%s\n", errbuf);
        free(errbuf);
        return errcode;
    }

    if (NULL == (pmatch = malloc(sizeof(regmatch_t) * NMATCH))) {
        regfree(&preg);
        free(errbuf);
        perror("");
        return errno;
    }

    subs = argv[2];
    subs = process_double_backslash(subs);
    if (NULL == subs) {
        regfree(&preg);
        free(errbuf);
        free(pmatch);
        perror("");
        return errno;
    }

    if (-1 == (n_links = check_sub_links(subs, preg.re_nsub))) {
        regfree(&preg);
        free(errbuf);
        free(pmatch);
        free(subs);
        fprintf(stderr, "error: invalid substitution string\n");
        return -1;
    }

    if (NULL == (sub_links = parse_sub_links(subs, n_links))) {
        regfree(&preg);
        free(errbuf);
        free(pmatch);
        free(subs);
        perror("");
        return errno;
    }

    orig_len = strlen(argv[3]);
    if (NULL == (res = malloc(sizeof(char) * orig_len))) {
        regfree(&preg);
        free(errbuf);
        free(pmatch);
        free(subs);
        free(sub_links);
        perror("");
        return errno;
    }
    res[0] = 0;

    shift = 0;
    i = 0;
    res_len = 0;
    while (!(errcode = regexec(&preg, argv[3] + shift, NMATCH, pmatch, 0))) {
        if (res_len + pmatch[0].rm_so < orig_len) {
            /* Increase buffer size */
            orig_len *= 2;
            if (NULL == (tmp_res = malloc(sizeof(char) * orig_len))) {
                regfree(&preg);
                free(errbuf);
                free(pmatch);
                free(subs);
                free(sub_links);
                free(res);
                perror("");
                return errno;
            }
            strcpy(tmp_res, res);
            res = tmp_res;
            tmp_res = NULL;
        }

        for (int j = 0; j < pmatch[0].rm_so; ++j) {
            res[i++] = *(argv[3] + shift + j);
        }
        res[i] = 0;
        res_len += pmatch[0].rm_so;

        if (NULL == (subs_res = substitute(sub_links, n_links, subs, argv[3] + shift,
                                           pmatch))) {
            regfree(&preg);
            free(errbuf);
            free(pmatch);
            free(subs);
            free(sub_links);
            free(res);
            perror("");
            return errno;
        }
        subs_len = strlen(subs_res);

        if (res_len + subs_len < orig_len) {
            /* Increase buffer size */
            orig_len *= 2;
            if (NULL == (tmp_res = malloc(sizeof(char) * orig_len))) {
                regfree(&preg);
                free(errbuf);
                free(pmatch);
                free(subs);
                free(sub_links);
                free(res);
                free(subs_res);
                perror("");
                return errno;
            }
            strcpy(tmp_res, res);
            res = tmp_res;
            tmp_res = NULL;
        }
        strcat(res, subs_res);

        shift += pmatch[0].rm_eo;
        i += subs_len;
        res_len += subs_len;
        free(subs_res);
    }

    printf("%s\n", res);

    regfree(&preg);
    free(errbuf);
    free(pmatch);
    free(subs);
    free(sub_links);
    free(res);
}