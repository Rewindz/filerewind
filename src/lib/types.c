#include "types.h"




StringArray new_string_array(int capacity)
{
  StringArray stray = {0};
  stray.arr = calloc(capacity, sizeof(char *));
  stray.capacity = capacity;
  return stray;
 }

void string_array_free(StringArray *array)
{
  for(int i = 0; i < array->count; ++i){
    free(array->arr[i]);
  }
  free(array->arr);
  array->count = 0;
  array->capacity = -1;
}

void string_array_append(StringArray *array, const char* src)
{
  if(array->count + 1 >= array->capacity)
    return;

  array->arr[++array->count] = strdup(src);
  
}




