/*
 * string_util.h
 *
 * Functions that implement string operations.
 *
 */

#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <stdbool.h>

/**
 * Write the lower-case version of the source
 * string to the destination string. Source and
 * destination strings can be the same.
 *
 * @param dest destination string
 * @param src source string
 */
char *strlower(char *dest, const char *src);

/**
 * Returns true if http_src string ends with endswith string.
 *
 * @param src source string
 * @param endswith ends with string
 * @return true if http_src ends with endswith
 */
bool strendswith(const char *src, const char *endswith);

/**
 * Trims newline sequences '"\r\n" or "\n".
 *
 * @param src source string
 * @return true if string was trimmed
 */
bool trim_newline(char *src);

/**
 * Trims leading tabs, '\t', from a string
 *
 * @param src source string
 * @return the source string without leading tabs
 */
char* trim_tabs(const char *src);

#endif /* STRING_UTIL_H_ */
