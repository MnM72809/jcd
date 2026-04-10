#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int determineLength(int argc, char* argv[])
{
  int len = 0;
	for (int i = 1; i < argc; i++)
	{
		len += strlen(argv[i]);
	}
	return len;
}

void joinArguments(int argc, char* argv[], char* joined)
{
	for (int i = 1; i < argc; i++)
	{
		strcat(joined, argv[i]);
	}
}

int main(int argc, char* argv[])
{
  if (argc == 1) exit(0);
	// Determine (__) length
	int len = determineLength(argc, argv);
	// Join arguments with slashes
	char joined[len];
	joinArguments(argc, argv, joined);
	printf("Joined: %s\n", joined);
}
