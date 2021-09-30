#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DX 3

struct Text {
    char **data;
    int num_lines;
    int max_line_len;
};

int read_text(const char *path, struct Text *text) {
    FILE *file;
    char *data;
    struct stat sb;
    int line_start = 0;
    int line_idx = 0;
    int line_len = 0;

    if (NULL == (file = fopen(path, "r"))) {
        return -1;
    }

    // read raw data
    stat(path, &sb);
    if (NULL == (data = malloc(sizeof(char) * sb.st_size))) {
        return -1;
    }
    fread(data, sizeof(char), sb.st_size, file);
    fclose(file);

    // count lines
    text->num_lines = 0;
    for (int i = 0; i < sb.st_size; ++i) {
        if ('\n' == data[i]) {
            ++(text->num_lines);
        }
    }
    ++(text->num_lines);

    // split into lines
    if (NULL == (text->data = malloc(sizeof(char *) * text->num_lines))) {
        return -1;
    }
    text->max_line_len = 0;
    for (int i = 0; i < sb.st_size; ++i) {
        if ('\n' == data[i]) {
            line_len = i - line_start + 1;
            if (line_len > text->max_line_len) {
                text->max_line_len = line_len;
            }
            if (NULL ==
                ((text->data)[line_idx] = malloc(sizeof(char) * line_len))) {
                return -1;
            }
            memcpy((text->data)[line_idx], data + line_start, line_len - 1);
            (text->data)[line_idx][line_len - 1] = '\0';
            line_start = i + 1;
            ++line_idx;
        }
    }
    line_len = sb.st_size - line_start + 1;
    if (line_len > text->max_line_len) {
        text->max_line_len = line_len;
    }
    if (NULL == ((text->data)[line_idx] = malloc(sizeof(char) * line_len))) {
        return -1;
    }
    memcpy((text->data)[line_idx], data + line_start, line_len - 1);
    (text->data)[line_idx][line_len - 1] = 0;
    free(data);
    return 0;
}

int min(int x, int y) { return x < y ? x : y; }

void draw_lines(WINDOW *win, struct Text text, int start, int num_lines) {
    char *line;

    if (start > text.num_lines) {
        return;
    }
    if (NULL == (line = malloc(sizeof(char) * (text.max_line_len + 12)))) {
        return;
    }
    werase(win);
    int end = min(start + num_lines, text.num_lines);
    for (int i = start; i < end; ++i) {
        sprintf(line, "%d: %s", i + 1, text.data[i]);
        mvwaddnstr(win, i + 1 - start, 1, line, -1);
    }
    box(win, 0, 0);
    free(line);
    wrefresh(win);
}

int main(int argc, char **argv) {
    WINDOW *win;
    struct Text text;
    int c = 0;
    int shift = 0;

    if (argc != 2) {
        printf("%s", "Usage: ./Show path_to_text_file\n");
        return -1;
    }
    if (-1 == read_text(argv[1], &text)) {
        printf("%s %s\n", "Could not read file", argv[1]);
        return -1;
    }

    initscr(); // init ncurses
    noecho();  // disable display of entered character
    cbreak();  // read entered character right after pressing the button

    printw("File: %s; Size: %d", argv[1], text.num_lines);
    refresh();

    // create window
    win = newwin(LINES - 2 * DX, COLS - 2 * DX, DX, DX);
    keypad(win, TRUE);
    scrollok(win, TRUE);
    box(win, 0, 0);

    // read entered characters
    draw_lines(win, text, 0, LINES - 2 * DX);
    while (27 != (c = wgetch(win))) {
        if (' ' != c) {
            continue;
        }
        shift += LINES - 2 * DX - 2;
        draw_lines(win, text, shift, LINES - 2 * DX);
    }

    // delete window
    delwin(win);
    endwin();

    // free memory
    for (int i = 0; i < text.num_lines; ++i) {
        free(text.data[i]);
    }
    free(text.data);
}