#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>  // for struct password

#include "../include/utils.h"

#define INIT_SIZE 10

typedef struct {
    char *name;
    struct stat st;
} FileEntry;

void    get_permissions(mode_t mode, char *perm_str);
void    print_size(off_t size);
const char *get_color(mode_t mode);

void    free_arr(FileEntry *entries, size_t file_count) {
    for (size_t i = 0; i < file_count; ++i) {
        free(entries[i].name);
    }
    free(entries);
}

void    get_permissions(mode_t mode, char *perm_str) {
    perm_str[0] = S_ISDIR(mode) ? 'd' :
                  S_ISLNK(mode) ? 'l' :
                  S_ISCHR(mode) ? 'c' :
                  S_ISBLK(mode) ? 'b' :
                  S_ISFIFO(mode) ? 'p' :
                  S_ISSOCK(mode) ? 's' : '-';

    perm_str[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm_str[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm_str[3] = (mode & S_IXUSR) ? 'x' : '-';
    perm_str[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm_str[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm_str[6] = (mode & S_IXGRP) ? 'x' : '-';
    perm_str[7] = (mode & S_IROTH) ? 'r' : '-';
    perm_str[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm_str[9] = (mode & S_IXOTH) ? 'x' : '-';
    perm_str[10] = '\0'; // null terminator
}

void    print_size(off_t size) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int i = 0;
    double fsize = size;

    while (fsize >= 1024 && i < 4) {
        fsize /= 1024;
        i++;
    }

    printf("%.1f%s", fsize, units[i]);
}

const char *get_color(mode_t mode) {
    if (S_ISDIR(mode)) return DIR_COR;
    if (S_ISREG(mode) && (mode & S_IXUSR)) return EXEC_COR;
    return REGULAR_COR;
}

void    print_entry(FileEntry *entry, bool long_format, bool human_readable, bool newline) {
    char permissions[11];
    const char *color = get_color(entry->st.st_mode);
    get_permissions(entry->st.st_mode, permissions);
    
    // Get file owner
    struct passwd *owner_info = getpwuid(entry->st.st_uid); // uid_t owner_uid = entry->st.st_uid;
    if (!owner_info) {
        die(errno == 0 ? "No user found for UID" : "getpwuid error");
    }

    if (long_format) {
        //problem with entry->
        printf("%s %s %s%s%s ", permissions, (char *)owner_info->pw_name, color, entry->name, RST_COR);
        human_readable ? print_size(entry->st.st_size) : printf("%ld", entry->st.st_size);
        if (newline) printf("\n");
    } else {
        printf("%s%s%s", color, entry->name, RST_COR);
        if (newline) printf("\n");
    }
}

int     traverse_directory(const char *path, bool show_all, FileEntry **entries_ptr, size_t *file_count, size_t *capacity) {
    DIR *dir = opendir(path);
    if (!dir) die("Unable to open dir");
    struct dirent *entry;
    struct stat path_stat;
    FileEntry *entries = *entries_ptr;

    while ((entry = readdir(dir)) != NULL) {
        // *** stat(entry->d_name, &path_stat);

        //bool is_dot = strcmp(entry->d_name, ".") == 0;
        //bool is_dotdot = strcmp(entry->d_name, "..") == 0;

        if (!show_all && entry->d_name[0] == '.')
            continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &path_stat) == 0) {
            if (*file_count >= *capacity) {
                size_t new_capacity = (*capacity < 1024) ? *capacity * 2 : *capacity + 128;
                FileEntry *new_entries = realloc(entries, new_capacity * sizeof(FileEntry));
                if (!new_entries) {
                    // Free existing names to avoid memory leaks
                    free_arr(entries, *file_count);
                    die("Realloc failed");
               }
                entries = new_entries;
                *capacity = new_capacity;
            }

            // Allocate memory for the file name (copy)
            entries[*file_count].name = o_strdup(entry->d_name);
            entries[*file_count].st = path_stat;
            (*file_count)++;
            
            /* Just for practice
            if (S_ISDIR(path_stat.st_mode) && !is_dot && !is_dotdot) {
                traverse_directory(full_path, show_all, &entries, file_count, capacity);
            } */
        } 
    }
    closedir(dir);
    *entries_ptr = entries;
    return 0;
}

int     main(int argc, char *argv[]) {
    char *path = (argc < 3) ? "." : NULL;

    // Flags
    bool show_all = false, long_format = false, human_readable = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0) {
            help(argv);
        }
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'l': long_format = true; break;
                    case 'a': show_all = true; break;
                    case 'h': human_readable = true; break;
                    default: printf("Unknown flag -%c\n", argv[i][j]); break;
                }
            }
        } else {
            path = argv[i];
        }
    }

    FileEntry *entries = malloc(INIT_SIZE * sizeof(FileEntry));
    if (!entries) die("Malloc failed");

    size_t file_count = 0, capacity = INIT_SIZE;
    
    traverse_directory(path, show_all, &entries, &file_count, &capacity);

    size_t max_len = 0;
    for (size_t i = 0; i < file_count; i++) {
        // print_function for each and every file based in terminal
        size_t len = strlen(entries[i].name);
        if (len > max_len) max_len = len;
    }

    // Calculating based in terminal width
    int term_width = get_term_width();
    int col_width = max_len + 2; // 2 is the padding
    size_t cols = (term_width > 0 && col_width > 0) ? term_width / col_width : 1;
    if (cols == 0) cols = 1;

    size_t rows = (file_count + cols - 1) / cols;
    
    if (long_format) {
        for (size_t i = 0; i < file_count; i++) {
            print_entry(&entries[i], long_format, human_readable, true);
        }
    } else {
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                size_t i = c * rows + r;
                if (i < file_count) {
                    printf("%s%-*s%s", get_color(entries[i].st.st_mode), col_width, entries[i].name, RST_COR);
                }
            }
            printf("\n");
        }
    }

    free_arr(entries, file_count);
    return 0;
}
