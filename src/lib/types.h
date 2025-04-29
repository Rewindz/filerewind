#ifndef REW_TYPES_H_
#define REW_TYPES_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
  char **arr;
  int count;
  int capacity;
}StringArray;

StringArray new_string_array(int capacity);
void string_array_free(StringArray *array);
void string_array_append(StringArray *array, const char* src);


#endif //REW_TYPES_H_
