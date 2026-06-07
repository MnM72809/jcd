#pragma once

typedef struct {
  char** array;
  size_t count;
} charArray;

charArray splitToSegments(size_t inputCount, char** inputArray);
