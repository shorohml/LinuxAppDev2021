#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFSIZE 4096

/*
Check if paths point to the same file
Return 0 if files are not the same or one/both of the files doesn't exist
*/
int is_same_file(const char *path_1, const char *path_2) {
    struct stat file_1_st, file_2_st;

    if (0 == strcmp(path_1, path_2)) {
        return 1;
    }
    if (-1 == (stat(path_1, &file_1_st))) {
        return 0;
    }
    if (-1 == (stat(path_2, &file_2_st))) {
        return 0;
    }
    return file_1_st.st_dev == file_2_st.st_dev &&
           file_1_st.st_ino == file_2_st.st_ino;
}

/* Cleanup:
1. Free buffer
2. Close input and output files
3. Remove file, defined by path
*/
int cleanup(char *buffer, FILE *infile, FILE *outfile, const char *path) {
    free(buffer);
    if (0 != fclose(infile)) {
        perror("infile");
        return errno;
    }
    if (0 != fclose(outfile)) {
        perror("outfile");
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
    if (is_same_file(inpath, outpath)) {
        fprintf(stderr, "%s and %s are the same files\n", inpath, outpath);
        return -1;
    }

    /* open infile  */
    if (NULL == (infile = fopen(inpath, "r"))) {
        perror(inpath);
        return errno;
    }

    /* open outfile  */
    if (NULL == (outfile = fopen(outpath, "w+"))) {
        if (0 != fclose(infile)) {
            perror(inpath);
            return errno;
        }
        perror(outpath);
        return errno;
    }

    /* allocate buffer */
    if (NULL == (buffer = (char *)malloc(sizeof(char) * BUFFSIZE))) {
        perror("buffer");
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