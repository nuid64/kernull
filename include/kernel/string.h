#pragma once

#include "kernel/types.h"
#include <stddef.h>

/* Search in S for C */
void  *memchr(const void *s, int c, size_t n);

/* Compare N bytes of S1 and S2 */
int    memcmp(const void *s1, const void *s2, size_t n);

/* Copy N bytes of SRC to DEST */
void  *memcpy(void *restrict dest, const void *src, size_t n);

/* Copy N bytes of SRC to DEST, guaranteeing
   correct behavior for overlapping strings */
void  *memmove(void *dest, const void *src, size_t n);

/* Set N bytes of S to C */
void  *memset(void *s, int c, size_t n);

/* Append SRC onto DEST */
char    *strcat(char *restrict dest, const char *restrict src);
/* Append no more than N characters from SRC onto DEST.  */
char    *strncat(char *restrict dest, const char *restrict src, size_t n);

/* Search in S for C until terminating zero */
char    *strchr(const char *s, int c);

/* Compare S1 and S2 */
int    strcmp(const char *s1, const char *s2);
/* Compare N characters of S1 and S2 */
int    strncmp(const char *s1, const char *s2, size_t n);

/* Copy SRC to DEST */
char    *strcpy(char *restrict dest, const char *restrict src);
/* Copy no more than N characters of SRC to DEST */
char    *strncpy(char *restrict dest, const char *restrict src, size_t n);

/* Return the length of S */
size_t strlen(const char *s);

/* Find the last occurrence of C in S */
char    *strrchr(const char *s, int c);

/* Find the first substring */
char    *strstr(const char *dom, const char *sub);
