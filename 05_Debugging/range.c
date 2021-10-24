#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int m = 0, n, s = 1;

    switch (argc) {
    case 1:
        printf("Usage: ./main {N | M N | M N S}\n");
        return 0;
    case 2:
        n = atoi(argv[1]);
        break;
    case 3:
        m = atoi(argv[1]);
        n = atoi(argv[2]);
        break;
    case 4:
        m = atoi(argv[1]);
        n = atoi(argv[2]);
        s = atoi(argv[3]);
        break;
    }
    if (s == 0 || m > n && s > 0 || m < n && s < 0) {
        printf("Invalid M N S combination: %d %d %d\n", m, n, s);
        return 0;
    }
    for (int i = m; i < n; i += s) {
        printf("%d\n", i);
    }
    return 0;
}