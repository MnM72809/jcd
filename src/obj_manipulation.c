#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj_manipulation.h"

charArray splitToSegments(size_t inputCount, char** inputArray)
{
	charArray splitArray = {
		.array = NULL,
		.count = 0
	};

	for (size_t i = 0; i < inputCount; i++)
	{
		char* str = strdup(inputArray[i]);
		char* token = strtok(str, "/");

		while (token != NULL)
		{
			splitArray.array = realloc(splitArray.array, (splitArray.count + 1) * sizeof(char*));
			splitArray.array[splitArray.count] = strdup(token);
			splitArray.count++;
			token = strtok(NULL, "/");
		}
		free(str);
	}

	return splitArray;
}
