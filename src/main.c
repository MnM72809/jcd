#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "algorithms.h"
#include "init.h"
#include "obj_manipulation.h"

#define ALLOWED_SCORE_DIVISOR 3

void addToPath(char *path, char *s)
{
    strncat(path, "/", PATH_MAX - strlen(path) - 1);
    strncat(path, s, PATH_MAX - strlen(path) - 1);
}

void run(int argc, char *argv[], char path[])
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

    // Exclude the program name from splitArray
    charArray splitArray = splitToSegments(argc - 2, &argv[2]);

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
    for (int i = 0; i < splitArray.count; i++)
    {
        for (int j = 0; j < strlen(splitArray.array[i]); j++)
            splitArray.array[i][j] = tolower(splitArray.array[i][j]);
    }

    // Loop through inputs
    for (int i = 0; i < splitArray.count; i++)
    {
        if (strcmp(splitArray.array[i], "..") == 0)
        {
            addToPath(path, "..");
            continue; // Don't interpret "..", just add it
        }

        struct dirent **namelist;
        int n;

        n = scandir(path, &namelist, NULL,
                    NULL); // not using alphasort because the list doesn't need to be sorted
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
            if (strcmp(namelist[j]->d_name, ".") == 0)
                continue; // Skip "."
            if (namelist[j]->d_type != DT_DIR)
                continue; // Skip non-directories

            // Convert to lowercase
            int lower_length = strlen(namelist[j]->d_name);
            char lowercase[lower_length + 1];
            lowercase[lower_length] = '\0';
            for (int k = 0; k < lower_length; k++)
            {
                lowercase[k] = tolower(namelist[j]->d_name[k]);
            }

            // Store the entry's score
            scores[j] = damerauLevenshtein(lowercase, splitArray.array[i], 256);
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

        if (lowest == -1 ||
            lowest > (strlen(namelist[lowest_index]->d_name) / ALLOWED_SCORE_DIVISOR))
        {
            fprintf(stderr, "jcd: no match found for %s, stopping at:\n%s\n", splitArray.array[i],
                    path);
            for (int j = 0; j < n; j++)
            {
                free(namelist[j]);
            }
            free(namelist);
            free(scores);
            break;
        }
        else
            addToPath(path, namelist[lowest_index]->d_name);

        /*printf("-> Lowest score: %u, value: %s\n", lowest, namelist[lowest_index]->d_name);*/

        for (int j = 0; j < n; j++)
        {
            free(namelist[j]);
        }
        free(namelist);
        free(scores);
    }

    for (int i = 0; i < splitArray.count; i++)
    {
        free(splitArray.array[i]);
    }
    free(splitArray.array);

    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "help") == 0)
    {
        printf("Usage: jcd [OPTIONS] <path>\n");
        printf("\n");
        printf("A fast way to navigate your filesystem.\n");
        printf("\n");
        printf("Options:\n");
        printf("  help     Print this help message and exit.\n");
        printf("  version  Print version information and exit.\n");
        printf(
            "  init     Print shell function initialization logic (use: 'eval \"$(jcd init)\")\n");
        /*printf("  cd       Print directory to change to (used for shell function)\n");*/
        exit(0);
    }

    if (strcmp(argv[1], "init") == 0)
    {
        init(argc, argv);
        exit(0);
    }

    if (strcmp(argv[1], "version") == 0)
    {
        printf("Version: 1.0.1\n");
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

    char path[PATH_MAX];
    run(argc, argv, path);
    printf("%s\n", path);
}
