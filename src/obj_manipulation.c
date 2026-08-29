#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj_manipulation.h"

charArray split_to_segments(size_t input_count, char **input_array)
{
    charArray split_array = {.array = NULL, .count = 0};

    for (size_t i = 0; i < input_count; i++)
    {
        char *str = strdup(input_array[i]);
        char *token = strtok(str, "/");

        while (token != NULL)
        {
            split_array.array =
                realloc(split_array.array, (split_array.count + 1) * sizeof(char *));
            if (split_array.array == NULL)
            {
                perror("Failed to allocate memory");
                exit(1);
            }
            split_array.array[split_array.count] = strdup(token);
            split_array.count++;
            token = strtok(NULL, "/");
        }
        free(str);
    }

    return split_array;
}
