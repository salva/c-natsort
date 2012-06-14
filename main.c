

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "natsort.h"

char **lines;
size_t next_line, lines_size;

static void
init_lines(void) {
    lines = malloc(sizeof(char *) * 100);
    lines_size = 10;
    next_line = 0;
}

static void
resize_lines() {
    size_t i;
    lines_size *= 2;
    lines = realloc(lines, sizeof(char *) * lines_size);
    if (!lines) {
        perror("resize_lines");
        exit(1);
    }
}

static void
read_lines(FILE *fp) {
    char *line;
    while(1) {
        char *line = NULL;
        size_t size;
        if (getline(&line, &size, fp) < 0) {
            if (feof(fp)) return;
            perror("read_lines");
            exit(1);
        }
        if (next_line >= lines_size) resize_lines();
        lines[next_line++] = line;
    }
}

int
main(int argc, char *argv[]) {
    size_t i;
    int opt, flags = 0;
    int debug = 0;

    while ((opt = getopt(argc, argv, "isrcaAqd")) != -1) {
        switch(opt) {
        case 'i':
            flags |= NATSORT_IGNORE_CASE;
            break;

        case 's':
            flags |= NATSORT_IGNORE_SYMBOLS;
            break;

        case 'r':
            flags |= NATSORT_REVERSE;
            break;

        case 'c':
            flags |= NATSORT_SIMPLE;
            break;

        case 'a':
            flags |= NATSORT_ALTCMP;
            break;

        case 'A':
            flags |= NATSORT_ALTCMP2;
            break;
        case 'd':
            flags |= NATSORT_DEBUG;
            debug = 1;
            break;
        case 'q':
            fclose(stdout);
            stdout = fopen("/dev/null", "w");
            break;
        }
    }

    argc -= optind;
    argv += optind;

    init_lines();    

    if (!argc) read_lines(stdin);
    else {
        for (; argc; argc--, argv++) {
            FILE *fp = fopen(argv[0], "r");
            if (!fp) {
                perror("unable to open file");
                continue;
            }
            read_lines(fp);
            fclose(fp);
        }
    }

    if (debug) fprintf(stderr, "files loaded\n");

    natsort(lines, next_line, flags);

    for (i = 0; i < next_line; i++)
        fputs(lines[i], stdout);

    exit(0);
}
