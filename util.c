#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"
#include "check.h"

// void util_insert_book(lib_t *lib, int book)
// {
//   if(lib->books == NULL || lib->count % ALLOC_GRANULARITY == 0)
//   {
//     CHECK((lib->books = realloc(lib->books, (lib->count+ALLOC_GRANULARITY)*sizeof(int))) != NULL);
//   }
// 
//   lib->books[lib->count++] = book;
// }
// 
// // la même pour la struct plib!
// void util_insert_book_p(plib_t *lib, int book)
// {
//   if(lib->books == NULL || lib->count % ALLOC_GRANULARITY == 0)
//   {
//     CHECK((lib->books = realloc(lib->books, (lib->count+ALLOC_GRANULARITY)*sizeof(int))) != NULL);
//   }
// 
//   lib->books[lib->count++] = book;
// }



int util_distance(int r1, int c1, int r2, int c2) {
  return (r1 > r2 ? r1 - r2 : r2 - r1) + (c1 > c2 ? c1 - c2 : c2 - c1);
}

void util_shuffle_array(void* array, int n, int size) {
  char buffer[size];
  char *a = array;

  if (n > 1) {
    for (int i = 0; i < n - 1; i++) {
      int rnd = (int) rand();
      int j = i + rnd / (RAND_MAX / (n - i) + 1);

      memcpy(buffer,       a + j * size, size);
      memcpy(a + j * size, a + i * size, size);
      memcpy(a + i * size, buffer,       size);
    }
  }
}

void util_write_score(char* output_file_name, int score) {
  char score_file_name[UTIL_BUFFER_MAX];
  FILE* output = NULL;
  snprintf(score_file_name, UTIL_BUFFER_MAX, "%s.score", output_file_name);
  CHECK((output = fopen(score_file_name, "w")) != NULL);
  CHECK(fprintf(output, "%d", score) > 0);
  CHECK(fclose(output) == 0);
}
