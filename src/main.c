#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "algorithms.h"
#include "init.h"
#include "obj_manipulation.h"

char path[PATH_MAX];
double allowed_score_divisor = 2.0;
char *back_str = "..";

void add_to_path(char *current_path, char *s)
{
    strncat(current_path, "/", PATH_MAX - strlen(current_path) - 1);
    strncat(current_path, s, PATH_MAX - strlen(current_path) - 1);
}

int dir_filter(const struct dirent *entry)
{
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        return 0;
    if (entry->d_type != DT_UNKNOWN)
    {
        if (entry->d_type == DT_DIR)
            return 1;
        if (entry->d_type != DT_LNK)
            return 0; // Fall back to stat logic if symlink
    }

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }

    if (errno == ENOENT)
        return 0;

    fprintf(stderr, "Error: Could not stat %s\nStopping at %s\n", full_path, path);
    return 0;
}

void run(int argc, char *argv[])
{
    // Print the home dir when no arguments are given
    if (argc == 2)
    {
        snprintf(path, PATH_MAX, "%s", getenv("HOME"));
        return;
    }

    bool relative = true;
    if (argv[2][0] == '/')
    {
        relative = false;
    }

    // Exclude the program name from split_array
    charArray split_array = split_to_segments(argc - 2, &argv[2]);

    if (relative)
    {
        // Store cwd in path
        if (getcwd(path, PATH_MAX) == NULL)
        {
            perror("getcwd() error");
            exit(1);
        }
    }
    else
    {
        path[0] = '/';
        path[1] = '\0';
    }

    // transform splitArray to lowercase
    for (int i = 0; i < split_array.count; i++)
    {
        for (int j = 0; j < strlen(split_array.array[i]); j++)
            split_array.array[i][j] = tolower(split_array.array[i][j]);
    }

    // Loop through inputs
    for (int i = 0; i < split_array.count; i++)
    {
        if (strcmp(split_array.array[i], ".") == 0)
            continue; // Skip "."
        if (strcmp(split_array.array[i], back_str) == 0 || strcmp(split_array.array[i], "..") == 0)
        {
            add_to_path(path, "..");
            continue; // Don't interpret "..", just add it
        }

        struct dirent **namelist;
        int n;

        n = scandir(path, &namelist, dir_filter, NULL);
        if (n < 0)
        {
            perror("scandir");
            exit(1);
        }

        unsigned *scores = malloc(n * sizeof(unsigned));
        if (scores == NULL)
        {
            perror("Failed to allocate memory");
            exit(1);
        }
        memset(scores, -1,
               n * sizeof(unsigned)); // memset is safe here cause -1 is 0xFF.. something (UINT_MAX)
        for (int j = 0; j < n; j++)
        {
            // Convert to lowercase
            int lower_length = strlen(namelist[j]->d_name);
            char lowercase[lower_length + 1];
            lowercase[lower_length] = '\0';
            for (int k = 0; k < lower_length; k++)
            {
                lowercase[k] = tolower(namelist[j]->d_name[k]);
            }

            // Store the entry's score
            scores[j] = damerau_levenshtein(lowercase, split_array.array[i], 256);
        }

        unsigned lowest = -1; // underflow uint
        int lowest_index = -1;
        for (int j = 0; j < n; j++)
        {
            if (scores[j] < lowest)
            {
                lowest = scores[j];
                lowest_index = j;
            }
        }

        int minimum_score = (double)strlen(namelist[lowest_index]->d_name) / allowed_score_divisor;
        if (lowest == -1 || (double)lowest > minimum_score)
        {
            fprintf(stderr, "jcd: no match found for \"%s\", stopping at:\n%s\n",
                    split_array.array[i], path);
            for (int j = 0; j < n; j++)
            {
                free(namelist[j]);
            }
            free(namelist);
            free(scores);
            break;
        }
        else
            add_to_path(path, namelist[lowest_index]->d_name);

        /*printf("-> Lowest score: %u, value: %s\n", lowest, namelist[lowest_index]->d_name);*/

        for (int j = 0; j < n; j++)
        {
            free(namelist[j]);
        }
        free(namelist);
        free(scores);
    }

    for (int i = 0; i < split_array.count; i++)
    {
        free(split_array.array[i]);
    }
    free(split_array.array);

    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "help") == 0)
    {
        printf("Usage: jcd <action> [OPTIONS]\n");
        printf("\n");
        printf("A fast way to navigate your filesystem.\n");
        printf("Add 'eval \"$(jcd init [OPTIONS])\"'\n");
        printf("to .bashrc or .zshrc to initialize\n");
        printf("\n");
        printf("Actions:\n");
        printf("  help     Print this help message and exit.\n");
        printf("  version  Print version information and exit.\n");
        printf("  init     Print shell function initialization logic.\n");
        printf("\n");
        printf("init options (used with 'jcd init'):\n");
        printf("  -a <alias>    Set the shell function name (default: j).\n");
        printf("  -b <backstr>  Set string to use as an alias for \"..\"\n");
        printf("                to navigate one directory level up.\n");
        printf("  -d <divisor>  Set the divisor for the allowed score threshold (default: 3.0).\n");
        printf(" 			    A lower divisor means stricter matching, a higher divisor means\n");
        printf("                looser matching.\n");
        printf("\n");
        exit(0);
    }

    if (strcmp(argv[1], "init") == 0)
    {
        init(argc, argv);
        exit(0);
    }

    if (strcmp(argv[1], "version") == 0)
    {
        printf("Version: 1.0.4\n");
        exit(0);
    }

    if (strcmp(argv[1], "cd") != 0)
    {
        printf("Unknown command: %s\n", argv[1]);
        exit(1);
    }

    if (argc == 3 && strcmp(argv[2], "-") == 0)
    {
        printf("-\n");
        exit(0);
    }

    // Check env var options
    char *back_str_env = getenv("JCD_BACK");
    if (back_str_env != NULL && strlen(back_str_env) != 0)
    {
        // Check if back_str_env doesn't contain any spaces
        if (strchr(back_str_env, ' ') != NULL)
        {
            fprintf(stderr, "Error: JCD_BACK cannot contain spaces.\n");
            exit(1);
        }
        back_str = back_str_env;
    }

    char *allowed_score_divisor_env = getenv("JCD_ALLOWED_SCORE_DIVISOR");
    if (allowed_score_divisor_env != NULL && strlen(allowed_score_divisor_env) != 0)
    {
        // Convert to int
        char *endptr;
        double val = strtod(allowed_score_divisor_env, &endptr);
        if (*endptr != '\0' || val <= 0)
        {
            fprintf(stderr, "Error: JCD_ALLOWED_SCORE_DIVISOR must be a positive number.\n");
            exit(1);
        }
        allowed_score_divisor = val;
    }

    run(argc, argv);
    printf("%s\n", path);
}
