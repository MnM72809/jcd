#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

#include "obj_manipulation.h"
#include "algorithms.h"

#define ALLOWED_SCORE_DIVISOR 3

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

	for (int i = 0; i < splitArray.count; i++) {
		for (int j = 0; j < strlen(splitArray.array[i]); j++)
			splitArray.array[i][j] = tolower(splitArray.array[i][j]);
	}

	// Loop through inputs
	for (int i = 0; i < splitArray.count; i++) {
		struct dirent **namelist;
		int n;

		n = scandir(path, &namelist, NULL, NULL); // not using alphasort because the list doesn't need to be sorted
		if (n < 0) {
			perror("scandir");
			exit(1);
		}

		unsigned *scores = malloc(n * sizeof(unsigned));
		if (scores == NULL) {
			perror("Failed to allocate memory");
			exit(1);
		}
		memset(scores, -1, n * sizeof(unsigned)); // memset is safe here cause -1 is 0xFF.. something (UINT_MAX)
		for (int j = 0; j < n; j++)
		{
			if (strcmp(namelist[j]->d_name, ".") == 0 || strcmp(namelist[j]->d_name, "..") == 0)
				continue; // Skip "." and ".."
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
			if (scores[j] < lowest) {
				lowest = scores[j];
				lowest_index = j;
			}
		}

		if (lowest == -1 || lowest > (strlen(namelist[lowest_index]->d_name) / ALLOWED_SCORE_DIVISOR))
		{
			printf("no match found for %s, skipping\n", splitArray.array[i]);
			/*printf("score: %u, allowed: %u\n", lowest, strlen(namelist[lowest_index]->d_name) / ALLOWED_SCORE_DIVISOR);*/
			// TODO: handle no match found properly
		}
		else {
			strncat(path, "/", PATH_MAX - strlen(path) - 1);
			strncat(path, namelist[lowest_index]->d_name, PATH_MAX - strlen(path) - 1);
		}

		/*printf("-> Lowest score: %u, value: %s\n", lowest, namelist[lowest_index]->d_name);*/

		for (int j = 0; j < n; j++) {
			free(namelist[j]);
		}
		free(namelist);
		free(scores);
	}


	printf("%s\n", path);

	// TODO: Cleanup splitArray
	// TODO: Cleanup namelist
}
