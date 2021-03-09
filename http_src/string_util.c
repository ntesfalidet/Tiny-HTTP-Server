/*
 * string_util.h
 *
 * Functions that implement string operations.
 *
 */

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "string_util.h"

/**
 * Write the lower-case version of the source
 * string to the destination string. Source and
 * destination strings can be the same.
 *
 * @param dest destination string
 * @param src source string
 */
char *strlower(char *dest, const char *src)
{
    int i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = tolower(src[i]);
    }
    dest[i] = '\0';
    return dest;
}

/**
 * Returns true if http_src string ends with endswith string.
 *
 * @param src source string
 * @param endswith ends with string
 * @return true if http_src ends with endswith
 */
bool strendswith(const char *src, const char *endswith) {
    size_t srclen = strlen(src);
    size_t endslen = strlen(endswith);
    return (srclen >= endslen) && (strcmp(src+srclen-endslen, endswith) == 0);
}

/**
 * Trims newline sequences '"\r\n" or "\n".
 *
 * @param src source string
 * @return true if string was trimmed
 */
bool trim_newline(char *src) {
    size_t srclen = strlen(src);

    if (srclen > 0) {
        if (src[srclen-1] == '\n') {
            src[srclen-1] = '\0';
            if ((srclen > 1) && src[srclen-2] == '\r') {
                src[srclen-2] = '\0';
            }
            return true;
        }
    }
    return false;
}

/**
 * Trims leading tabs, '\t', from a string
 *
 * @param src source string
 * @return the source string without leading tabs
 */
char* trim_tabs(const char *src) {

    static char str1[99];
    int count = 0, j, k;


    while (src[count] == '\t') {
        count++;
    }

    for (j = count, k = 0;
         src[j] != '\0'; j++, k++) {
        str1[k] = src[j];
    }
    str1[k] = '\0';

    return str1;
}
