#ifndef UTILS_H
#define UTILS_H

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#define LS_VERSION  0.01

// colors
#define REGULAR_COR "" // no color
#define EXEC_COR "\033[32m" // Green
#define DIR_COR "\033[1;34m" // Bold blue
#define RST_COR "\033[0m"


/* Wrappers */
void    *die(const char *msg);
int     Strcmp(const char *s1, const char *s2);
unsigned    get_term_width(void);
char*   o_strdup(const char *s);
void    help(char **argv);



































#endif
