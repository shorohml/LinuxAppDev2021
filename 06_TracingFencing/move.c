#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 4096

enum ERROR {
    SUCCESS = 0,
    WRONG_ARGS,
    SAME_FILE,
    MEMORY_ALLOCATION_ERROR,
    INFILE_OPEN_ERROR,
    OUTFILE_OPEN_ERROR,
    READ_ERROR,
    WRITE_ERROR,
    INFILE_CLOSE_ERROR,
    OUTFILE_CLOSE_ERROR,
    REMOVE_ERROR,
};

/* Cleanup:
1. Free buffer
2. Close input and output files
3. Remove file, defined by path
*/
enum ERROR cleanup(char *buffer, FILE *infile, FILE *outfile, char *path) {
    free(buffer);
    if (0 != fclose(infile)) {
        return INFILE_CLOSE_ERROR;
    }
    if (0 != fclose(outfile)) {
        return OUTFILE_CLOSE_ERROR;
    }
    if (0 != remove(path)) {
        return REMOVE_ERROR;
    }
    return SUCCESS;
}

/* Write string description of error to stderr */
void print_err(enum ERROR err_code, char *inpath, char *outpath) {
    switch (err_code) {
        case SUCCESS:
            return;
        case WRONG_ARGS:
            fprintf(stderr, "Usage: ./move infile outfile\n");
            break;
        case SAME_FILE:
            fprintf(stderr, "%s and %s is the same file\n", inpath, outpath);
            break;
        case MEMORY_ALLOCATION_ERROR:
            fprintf(stderr, "Error allocating memory\n");
            break;
        case INFILE_OPEN_ERROR:
            fprintf(stderr, "Error opening %s\n", inpath);
            break;
        case OUTFILE_OPEN_ERROR:
            fprintf(stderr, "Error opening %s\n", outpath);
            break;
        case READ_ERROR:
            fprintf(stderr, "Error reading from %s\n", inpath);
            break;
        case WRITE_ERROR:
            fprintf(stderr, "Error writing to %s\n", outpath);
            break;
        case INFILE_CLOSE_ERROR:
            fprintf(stderr, "Error closing %s\n", inpath);
            break;
        case OUTFILE_CLOSE_ERROR:
            fprintf(stderr, "Error closing %s\n", outpath);
            break;
        case REMOVE_ERROR:
            fprintf(stderr, "Error deleting file\n");
            break;
        default:
            fprintf(stderr, "Some error\n");
    }
}

int main(int argc, char **argv) {
    FILE *infile, *outfile;
    char *buffer;
    char *inpath, *outpath;
    size_t n_read, n_wrote;
    enum ERROR err_code;

    if (argc != 3) {
        fprintf(stderr, "Usage: ./move infile outfile\n");
        return WRONG_ARGS;
    }
    inpath = argv[1];
    outpath = argv[2];

    /* Won't work for the same file */
    if (0 == strcmp(inpath, outpath)) {
        print_err(SAME_FILE, inpath, outpath);
        return SAME_FILE;
    }

    /* allocate buffer */
    if (NULL == (buffer = (char *)malloc(sizeof(char) * BUFFSIZE))) {
        print_err(MEMORY_ALLOCATION_ERROR, inpath, outpath);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* open infile  */
    if (NULL == (infile = fopen(inpath, "r"))) {
        free(buffer);
        print_err(INFILE_OPEN_ERROR, inpath, outpath);
        return INFILE_OPEN_ERROR;
    }

    /* open outfile  */
    if (NULL == (outfile = fopen(outpath, "w+"))) {
        free(buffer);
        if (0 != fclose(infile)) {
            print_err(INFILE_CLOSE_ERROR, inpath, outpath);
            return INFILE_CLOSE_ERROR;
        }
        print_err(OUTFILE_OPEN_ERROR, inpath, outpath);
        return OUTFILE_OPEN_ERROR;
    }

    /* copy blocks from infile to outfile */
    while (0 != (n_read = fread(buffer, sizeof(char), BUFFSIZE, infile))) {
        if (n_read != BUFFSIZE && ferror(infile)) {
            /* error in fread */
            err_code = cleanup(buffer, infile, outfile, outpath);
            if (err_code != SUCCESS) {
                print_err(err_code, inpath, outpath);
                return err_code;
            }
            print_err(READ_ERROR, inpath, outpath);
            return READ_ERROR;
        }
        n_wrote = fwrite(buffer, sizeof(char), n_read, outfile);
        if (n_wrote != n_read) {
            /* error in fwrite */
            err_code = cleanup(buffer, infile, outfile, outpath);
            if (err_code != SUCCESS) {
                print_err(err_code, inpath, outpath);
                return err_code;
            }
            print_err(WRITE_ERROR, inpath, outpath);
            return WRITE_ERROR;
        }
    }

    err_code = cleanup(buffer, infile, outfile, inpath);
    if (err_code != SUCCESS) {
        print_err(err_code, inpath, outpath);
        return err_code;
    }
    return SUCCESS;
}