#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFFSIZE 4096

/* Cleanup:
1. Free buffer
2. Close input and output files
3. Remove file, defined by path
*/
int cleanup(char *buffer, FILE *infile, FILE *outfile, const char *path) {
    free(buffer);
    if (0 != fclose(infile)) {
        perror("");
        return errno;
    }
    if (0 != fclose(outfile)) {
        perror("");
        return errno;
    }
    if (0 != remove(path)) {
        perror(path);
        return errno;
    }
    return 0;
}

int main(int argc, char **argv) {
    FILE *infile, *outfile;
    char *buffer;
    char *inpath, *outpath;
    size_t n_read, n_wrote;
    int err_code;

    if (argc != 3) {
        fprintf(stderr, "Usage: ./move infile outfile\n");
        return -1;
    }
    inpath = argv[1];
    outpath = argv[2];

    /* won't work for the same file */
    if (0 == strcmp(inpath, outpath)) {
        fprintf(stderr, "%s and %s are the same files\n", inpath, outpath);
        return -1;
    }

    /* allocate buffer */
    if (NULL == (buffer = (char *)malloc(sizeof(char) * BUFFSIZE))) {
        perror("");
        return errno;
    }

    /* open infile  */
    if (NULL == (infile = fopen(inpath, "r"))) {
        free(buffer);
        perror(inpath);
        return errno;
    }

    /* open outfile  */
    if (NULL == (outfile = fopen(outpath, "w+"))) {
        free(buffer);
        if (0 != fclose(infile)) {
            perror(inpath);
            return errno;
        }
        perror(outpath);
        return errno;
    }

    /* copy blocks from infile to outfile */
    while (0 != (n_read = fread(buffer, sizeof(char), BUFFSIZE, infile))) {
        if (n_read != BUFFSIZE && ferror(infile)) {
            /* error in fread */
            err_code = cleanup(buffer, infile, outfile, outpath);
            if (0 != err_code) {
                return err_code;
            }
            perror("read");
            return errno;
        }
        n_wrote = fwrite(buffer, sizeof(char), n_read, outfile);
        if (n_wrote != n_read) {
            /* error in fwrite */
            err_code = cleanup(buffer, infile, outfile, outpath);
            if (0 != err_code) {
                return err_code;
            }
            perror("write");
            return errno;
        }
    }

    err_code = cleanup(buffer, infile, outfile, inpath);
    if (0 != err_code) {
        return errno;
    }
    return 0;
}