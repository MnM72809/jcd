#pragma once
#include <stdlib.h>

typedef struct
{
    char **array;
    size_t count;
} charArray;

charArray split_to_segments(size_t input_count, char **input_array);
