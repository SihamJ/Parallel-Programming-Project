#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include "solution.h"
#include "problem.h"

#define UTIL_BUFFER_MAX 32768

# define UTIL_msg(msg)                                                     \
         do {                                                              \
           fprintf(stderr,"[solver] " msg "\n");                           \
         } while (0)


# define UTIL_msg_begin(msg)                                               \
         do {                                                              \
           fprintf(stderr,"[solver] " msg);                                \
           fflush(stderr);                                                 \
         } while (0)


# define UTIL_msg_end(msg)                                                 \
         do {                                                              \
           fprintf(stderr, msg "\n");                                      \
         } while (0)

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define ALLOC_GRANULARITY 10000

// void util_insert_book(lib_t *lib, int book);
// void util_insert_book_p(plib_t *lib, int book);
// int util_distance(int, int, int, int);
// void   util_shuffle_array(void*, int, int);
void   util_write_score(char*, int);

#endif /* __UTIL_H__ */
