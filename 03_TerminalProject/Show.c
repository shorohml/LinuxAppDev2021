#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DX 3

struct Text {
    char **data;
    int *len;
    int num_lines;
    int max_line_len;
};

int min(int x, int y) { return x < y ? x : y; }

int max(int x, int y) { return x < y ? y : x; }

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

    if (NULL == (text->len = malloc(sizeof(int) * text->num_lines))) {
        return -1;
    }
    for (int i = 0; i < text->num_lines; ++i) {
        text->len[i] = strlen(text->data[i]);
    }

    free(data);
    return 0;
}

void free_text(struct Text text) {
    for (int i = 0; i < text.num_lines; ++i) {
        free(text.data[i]);
    }
    free(text.data);
    free(text.len);
}

void draw_lines(WINDOW *win, struct Text text, char *line, int x, int y,
                int width, int height) {
    if (y > text.num_lines || y < 0) {
        return;
    }
    werase(win);
    int end = min(y + height, text.num_lines);
    for (int i = y; i < end; ++i) {
        int shift = min(x, text.len[i]);
        sprintf(line, " %d: %s", i + 1, text.data[i] + shift);
        mvwaddnstr(win, i + 1 - y, 1, line, width);
    }
    box(win, 0, 0);
    wrefresh(win);
}

int main(int argc, char **argv) {
    WINDOW *win;
    struct Text text;
    char *line;
    int c = 0;
    int x = 0, y = 0;
    int next;

    if (argc != 2) {
        printf("%s", "Usage: ./Show path_to_text_file\n");
        return -1;
    }
    if (-1 == read_text(argv[1], &text)) {
        printf("%s %s\n", "Could not read file", argv[1]);
        return -1;
    }

    initscr();   // init ncurses
    noecho();    // disable display of entered character
    cbreak();    // read entered character right after pressing the button
    curs_set(0); // hide cursor

    printw("File: %s; Size: %d", argv[1], text.num_lines);
    refresh();

    // create window
    win = newwin(LINES - 2 * DX, COLS - 2 * DX, DX, DX);
    keypad(win, TRUE);
    scrollok(win, TRUE);
    box(win, 0, 0);

    if (NULL == (line = malloc(sizeof(char) * (text.max_line_len + 13)))) {
        return -1;
    }

    // read entered characters
    draw_lines(win, text, line, 0, 0, COLS - 2 * DX, LINES - 2 * DX);
    while (27 != (c = wgetch(win))) {
        switch (c) {
        case ' ':
        case KEY_NPAGE:
            next = y + LINES - 2 * DX - 2;
            if (next < text.num_lines) {
                y = next;
            }
            break;
        case KEY_PPAGE:
            y -= LINES - 2 * DX - 2;
            break;
        case KEY_DOWN:
            y += 1;
            break;
        case KEY_UP:
            y -= 1;
            break;
        case KEY_RIGHT:
            x += 1;
            break;
        case KEY_LEFT:
            x -= 1;
            break;
        default:
            continue;
        }
        y = max(min(y, text.num_lines - 1), 0);
        x = max(x, 0);
        draw_lines(win, text, line, x, y, COLS - 2 * DX, LINES - 2 * DX);
    }

    // delete window
    delwin(win);
    endwin();

    // free memory
    free(line);
    free_text(text);
}