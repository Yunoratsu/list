#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdlib.h>
#include <curses.h> // or <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <term.h>

#include "../include/utils.h"

void    *die(const char *msg) {
    if (errno) {
        perror(msg);
    } else {
        fprintf(stderr, "Error: %s\n", msg);
    }
    exit(1);
}

int     Strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2 && (*s1 != '\0')) {
        s1++;
        s2++;
    }

    // If string a is equal to string b return 0 else < 0 or > 0...
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

// Need to implement snprintf soon

/* To get the terminal width:
 *  Get the value of the TERM environment variable;
 *  The path of the current terminal with ctermid();
 *  Open the path with open()
 *  Get terminal data from terminfo db via setupterm();
 *  Get number of columns from tigetnum();
 */
unsigned    get_term_width(void) {
    char const *const term = getenv("TERM");
    if (term == NULL) {
        die("TERM environment variable not set");
    }

    char const *const cterm_path = ctermid(NULL);
    if (cterm_path == NULL || cterm_path[0] == '\0') {
        die("ctermid() failed");
    }

    int tty_fd = open(cterm_path, O_RDWR);
    if (tty_fd == -1) {
        die("open failed");
    }

    int cols = 0;
    int setupterm_err;
    if (setupterm((char *)term, tty_fd, &setupterm_err) == ERR) {
        switch(setupterm_err) {
            case -1:
                die("setupterm() failed: terminfo database not found");
                goto done;
            case 0:
                die("setupterm() failed: term not found in database");
                goto done;
            case 1:
                die("setupterm() failed: terminal is hardcopy");
                goto done;
        } // switch
    }

    cols = tigetnum((char *)"cols");
    if (cols < 0)
        die("tigetnum() failed");

done:
    if (tty_fd != -1)
        close(tty_fd);
    return cols < 0 ? 0 : cols;
}

char*       o_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

void    help(char **argv) {
    printf("LS_VERSION: %lf\n", LS_VERSION);
    printf("Usage: %s [options] [path]\n", argv[0]);
    printf("Options:\n");
    printf("\t-a    Show all files, including hidden\n");
    printf("\t-l    Long listing format\n");
    printf("\t-h    Human-readable file sizes\n");
    printf("\t-help or --help   Show this help message\n");
    printf("\tExamples:\n");
    printf("\t\t%s -alh /etc\n", argv[0]);
    exit(0);
}
