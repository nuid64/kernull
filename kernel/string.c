#include <kernel/string.h>

#include <kernel/types.h>

/* Search in S for C */
void *memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = s;
    while (n--) {
        if (*p == (unsigned char) c)
            return (void *) p;
        ++p;
    }

    return NULL;
}

/* Compare N bytes of S1 and S2 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    int ret = 0;
    while (n--) {
        if ((ret = *p1 - *p2) != 0)
            break;

        ++p1;
        ++p2;
    }

    return ret;
}

/* Copy N bytes of SRC to DEST */
void *memcpy(void *restrict dest, const void *src, size_t n)
{
    const char *ps = src;
    char *pd = dest;
    while (n--)
        *pd++ = *ps++;

    return dest;
}

/* Copy N bytes of SRC to DEST, guaranteeing
   correct behavior for overlapping strings */
void *memmove(void *dest, const void *src, size_t n)
{
    const char *ps = src;
    char *pd = dest;

    if (dest <= src) {
        while (n--)
            *pd++ = *ps++;
    } else {
        ps += n;
        pd += n;

        while (n--)
            *--pd = *--ps;
    }

    return dest;
}

/* Set N bytes of S to C */
void *memset(void *s, int c, size_t n)
{
    asm (
        "cld; rep stosb"
        : "=c"((int){0})
        : "D"(s), "a"(c), "c"(n)
        : "flags", "memory"
    );
    return s;
}

/* Append SRC onto DEST */
char *strcat(char *restrict dest, const char *restrict src)
{
    char *ret = dest;

    while (*dest++)
        ;
    while ((*dest++ = *src++) != '\0')
        ;

    return ret;
}

/* Append no more than N characters from SRC onto DEST.  */
char *strncat(char *restrict dest, const char *restrict src, size_t n)
{
    char *ret = dest;

    if (n) {
        while (*dest++)
            ;
        while ((*dest++ = *src++) != '\0')
            if (--n == 0) {
                *dest = '\0';
                break;
            }
    }

    return ret;
}

/* Search in S for C until NUL byte */
char *strchr(const char *s, int c)
{
    while (*s != '\0') {
        if (*s == (unsigned char) c)
            return (char *) s;
        ++s;
    }

    return NULL;
}

/* Compare S1 and S2 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2)
            return *s1 < *s2? -1 : 1;

        ++s1;
        ++s2;
    }

    return 0;
}
/* Compare N characters of S1 and S2 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && *s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2)
            return *s1 < *s2? -1 : 1;

        ++s1;
        ++s2;
    }

    return 0;
}

/* Copy SRC to DEST */
char *strcpy(char *restrict dest, const char *restrict src)
{
    char *ret = dest;

    while ((*dest++ = *src++) != '\0')
        ;

    return ret;
}
/* Copy no more than N characters of SRC to DEST */
char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
    char *ret = dest;

    while (n--) {
        if ((*dest++ = *src++) == '\0')
            break;
    }

    return ret;
}

/* Return the length of S */
size_t strlen(const char *s)
{
    size_t n = 0;
    while (*(s + n) != '\0')
        ++n;

    return n;
}

/* Find the last occurrence of C in S */
char *strrchr(const char *s, int c)
{
    char *ret = NULL;
    while (*s != '\0') {
        if (*s == (unsigned char) c)
            ret = (char *) s;
        ++s;
    }

    return ret;
}

/* Find the first substring */
char *strstr(const char *dom, const char *sub)
{
    size_t sub_len = strlen(sub);
    if (sub_len == 0)
        return (char *) dom;

    size_t dom_len = strlen(dom);

    while (dom_len >= sub_len) {
        dom_len--;
        if (memcmp(dom, sub, sub_len) == 0)
            return (char *) dom;
        ++dom;
    }

    return NULL;
}
