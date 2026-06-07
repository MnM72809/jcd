#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#include "obj_manipulation.h"
#include "algorithms.h"


int determineLength(int argc, char* argv[])
{
  int len = 0;
	for (int i = 1; i < argc; i++)
	{
		len += strlen(argv[i]) + 1; // Room for the slashes
	}
	return len;
}

void joinArguments(int argc, char* argv[], char* joined)
{
	for (int i = 1; i < argc; i++)
	{
		strcat(joined, argv[i]);
		strcat(joined, "/");
	}
}

int main(int argc, char* argv[])
{
	// Print the home dir when no arguments are given
	if (argc == 1) {
		printf("%s\n", getenv("HOME"));
		exit(0);
	}

	// Exclude the program name from the splitArray
	charArray splitArray = splitToSegments(argc - 1, &argv[1]);

	// Store cwd in path
	char path[PATH_MAX];
	if (getcwd(path, sizeof(path)) == NULL)
	{
		perror("getcwd() error");
		exit(1);
	}

	/*printf("cwd: %s\n", path);*/
	for (int i = 0; i < splitArray.count; i++) {
		printf("> %s\n", splitArray.array[i]);
		for (int j = 0; j < strlen(splitArray.array[i]); j++)
			splitArray.array[i][j] = tolower(splitArray.array[i][j]);
	}

	// Loop through inputs
	for (int i = 0; i < splitArray.count; i++) {
		struct dirent **namelist;
		int n;

		n = scandir(path, &namelist, NULL, NULL); // not using alphasort because the list doesn't need to be sorted
		unsigned scores[n];
		for (int j = 0; j < n; j++)
		{
			if (strcmp(namelist[j]->d_name, ".") == 0 || strcmp(namelist[j]->d_name, "..") == 0)
				continue; // Skip "." and ".."
			/*printf("value: %s\n", namelist[j]->d_name);*/
			int lower_length = strlen(namelist[j]->d_name);
			char lowercase[lower_length];
			for (int k = 0; k < lower_length; k++)
			{
				lowercase[k] = tolower(namelist[j]->d_name[k]);
			}
			scores[j] = damerauLevenshtein(lowercase, splitArray.array[i], 256);
		}

		unsigned lowest = -1; // underflow uint
		int lowest_index = -1;
		for (int j = 0; j < n; j++)
		{
			if (scores[j] < lowest) {
				lowest = scores[j];
				lowest_index = j;
			}
		}

		printf("Lowest score: %u; index: %d; value: %s\n", lowest, lowest_index, namelist[lowest_index]->d_name);

		strncat(path, "/", PATH_MAX - strlen(path) - 1);
		strncat(path, namelist[lowest_index]->d_name, PATH_MAX - strlen(path) - 1);

		// TODO: cleanup namelist
	}


	/*const char* str1 = splitArray.array[0];*/
	/*const char* str2 = splitArray.array[1];*/
	/*unsigned distance = damerauLevenshtein(str1, str2, 256);*/
	/*printf("distance: %d\n", distance);*/
	printf("path: %s\n", path);

	// TODO: Cleanup splitArray
	// TODO: Cleanup namelist

	// TODO: search for segfaults (they do appear)
}
